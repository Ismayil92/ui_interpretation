#!/usr/bin/env python

from __future__ import print_function 
import cv2 as cv
import numpy as np 
import rospy 
import os
from sensor_msgs.msg import Image 
from cv_bridge import CvBridge, CvBridgeError 
from enum import Enum
from coordinates import *
from ui_interpretation.msg import localizationMsg



#Global Variables 
bridge = CvBridge()
rgbImg = np.zeros([640,480],dtype = np.uint8)
grayImg = np.zeros([640,480], dtype = np.uint8)
sceneRGBImg = np.zeros([640,480], dtype=np.uint8)
refRGBImg = np.zeros([640,480], dtype = np.uint8)
refGrayImg = np.zeros([640,480], dtype = np.uint8)
H = np.empty((3,3), dtype = np.float32)


def openRefImages(image_name):
    global refRGBImg 
    global refGrayImg
    abs_drc = os.path.abspath(__file__)
    path_to_scripts_ind = abs_drc.find('script')
    path_to_project = abs_drc[0:path_to_scripts_ind]
    PATH_TO_IMAGE = os.path.join(path_to_project, 'Data/images/', image_name)
    refRGBImg = cv.imread(PATH_TO_IMAGE, cv.IMREAD_COLOR)
    if refRGBImg is None:
            print('Could not open or find the image!')
            exit(0)
    refGrayImg = cv.cvtColor(refRGBImg,cv.COLOR_BGR2GRAY)
    
    

def frameCallback(data): #Subscribing Video Frames
    global rgbImg
    global grayImg
    try:
        rgbImg = bridge.imgmsg_to_cv2(data,"bgr8")
        grayImg = cv.cvtColor(rgbImg, cv.COLOR_BGR2GRAY)        
    except CvBridgeError as e:
        print(e)


class SURF_detector:
    _min_hessian = 400  
    _lowe_ratio_thresh = 0.75     
    
    def __init__(self):
        self.detector = cv.xfeatures2d.SURF_create(hessianThreshold=SURF_detector._min_hessian)
        self.keypoints_ref, self.descriptors_ref = self.detector.detectAndCompute(refGrayImg,None)
        self.matcher = cv.DescriptorMatcher_create(cv.DescriptorMatcher_FLANNBASED)

    def compute(self):
        keypoints_scene, descriptors_scene = self.detector.detectAndCompute(grayImg,None)
        #  Step 2: Matching descriptor vectors with a FLANN based matcher
        #  Since SURF is a floating-point descriptor NORM_L2 is used
        knn_matches = []
        try:
            knn_matches = self.matcher.knnMatch(self.descriptors_ref, descriptors_scene, 2)
        except:
            print("Error occured in knn matching")
        #  Filter matches using the Lowe's ratio test to eliminate outliers 
        good_matches = [] 
        for m,n in knn_matches:
            if m.distance < SURF_detector._lowe_ratio_thresh * n.distance:
                good_matches.append(m)
        #  Localize the object
        obj = np.empty((len(good_matches),2), dtype=np.float32)
        scene = np.empty((len(good_matches),2), dtype=np.float32)
        for i in range(len(good_matches)):
            #-- Get the keypoints from the good matches in both images
            obj[i,0] = self.keypoints_ref[good_matches[i].queryIdx].pt[0]
            obj[i,1] = self.keypoints_ref[good_matches[i].queryIdx].pt[1]
            scene[i,0] = keypoints_scene[good_matches[i].trainIdx].pt[0]
            scene[i,1] = keypoints_scene[good_matches[i].trainIdx].pt[1]

        # to find Homography Matrix 
        global H
        try:
            H, maskMatrix = cv.findHomography(obj, scene, cv.RANSAC)
            matches_mask = maskMatrix.ravel().tolist()
        except Exception as ex:
            print(ex)   
        return H    

def perspectiveTransform(ROI_in,H):
    ROI_scene = np.empty_like(ROI_in)
    try:
        for i in range(ROI_in.shape[0]):
            ROI_scene[i] = cv.perspectiveTransform(ROI_in[i],H)
    except:
        print("Problem occured in perspective transformation computation")
    return ROI_scene

def drawROI(ROI_scene,img_scene):
    color = (0,255,0)
    depth = 2 
    for i in range(ROI_scene.shape[0]):
        out_scene = cv.polylines(img_scene,[np.int32(ROI_scene[i])],True,color,depth)
    return out_scene


if __name__ == "__main__":
    rospy.init_node('coordinate_estimator',anonymous=True)
    rate = rospy.Rate(15)
    imageTopic = "/camera/color/image_raw"  
    coordPubTopic = "scene_coords"
    image_sub = rospy.Subscriber(imageTopic, Image, frameCallback, queue_size=1) #image subscriber
    coord_pub = rospy.Publisher(coordPubTopic, localizationMsg, queue_size=10) #scene coordiniates publisher
    openRefImages("refRGBImage.jpg") 
    surf = SURF_detector()
    refCorners = DesiredROIs()   
    scene_coords = localizationMsg()
    while not rospy.is_shutdown():
        H = surf.compute()                
        # Perspective Transform 
        hour_coord_scene = perspectiveTransform(refCorners.hour_corners, H)
        kg_coord_scene = perspectiveTransform(refCorners.kg_corners, H)
        temp_coord_scene = perspectiveTransform(refCorners.temp_corners, H)
        centrif_coord_scene = perspectiveTransform(refCorners.centr_corners, H)
        speed_coord_scene = perspectiveTransform(refCorners.speed_corners, H)
        symbol_coord_scene = perspectiveTransform(refCorners.symbol_corners, H)
        program_coord_scene = perspectiveTransform(refCorners.program_corners, H)
        option_coord_scene = perspectiveTransform(refCorners.option_corners, H)
        #Draw line to show ROIs
        sceneRGBImg = drawROI(hour_coord_scene,rgbImg)
        sceneRGBImg = drawROI(kg_coord_scene,rgbImg)
        sceneRGBImg = drawROI(temp_coord_scene,rgbImg)
        sceneRGBImg = drawROI(centrif_coord_scene,rgbImg)
        sceneRGBImg = drawROI(speed_coord_scene,rgbImg)
        sceneRGBImg = drawROI(symbol_coord_scene,rgbImg)
        sceneRGBImg = drawROI(program_coord_scene,rgbImg)
        sceneRGBImg = drawROI(option_coord_scene,rgbImg)       
        #Publish the scene coordinates          
        hour_coord_scene = np.asarray(hour_coord_scene,dtype=np.uint32).squeeze().tolist()
        kg_coord_scene = np.asarray(kg_coord_scene,dtype=np.uint32).squeeze().tolist()
        temp_coord_scene = np.asarray(temp_coord_scene,dtype=np.uint32).squeeze().tolist()
        centrif_coord_scene = np.asarray(centrif_coord_scene,dtype=np.uint32).squeeze().tolist()
        speed_coord_scene = np.asarray(speed_coord_scene,dtype=np.uint32).squeeze().tolist()
        symbol_coord_scene = np.asarray(symbol_coord_scene,dtype=np.uint32).squeeze().tolist()
        program_coord_scene = np.asarray(program_coord_scene,dtype=np.uint32).squeeze().tolist()
        option_coord_scene = np.asarray(option_coord_scene,dtype=np.uint32).squeeze().tolist()
        # DIGITS
        #Hour Message
        scene_coords.firsthourupleft = hour_coord_scene[0][0]
        scene_coords.firsthourupright = hour_coord_scene[0][1]
        scene_coords.firsthourbottomright = hour_coord_scene[0][2]
        scene_coords.firsthourbottomleft = hour_coord_scene[0][3]

        scene_coords.secondhourupleft = hour_coord_scene[1][0]
        scene_coords.secondhourupright = hour_coord_scene[1][1]
        scene_coords.secondhourbottomright = hour_coord_scene[1][2]
        scene_coords.secondhourbottomleft = hour_coord_scene[1][3]

        scene_coords.thirdhourupleft = hour_coord_scene[2][0]
        scene_coords.thirdhourupright = hour_coord_scene[2][1]
        scene_coords.thirdhourbottomright = hour_coord_scene[2][2]
        scene_coords.thirdhourbottomleft = hour_coord_scene[2][3]
        #Kg Message
        scene_coords.firstkgupleft = kg_coord_scene[0][0]
        scene_coords.firstkgupright = kg_coord_scene[0][1]
        scene_coords.firstkgbottomright = kg_coord_scene[0][2]
        scene_coords.firstkgbottomleft = kg_coord_scene[0][3]

        scene_coords.secondkgupleft = kg_coord_scene[1][0]
        scene_coords.secondkgupright = kg_coord_scene[1][1]
        scene_coords.secondkgbottomright = kg_coord_scene[1][2]
        scene_coords.secondkgbottomleft = kg_coord_scene[1][3]
        #Temp Message
        scene_coords.firsttempupleft = temp_coord_scene[0][0]
        scene_coords.firsttempupright = temp_coord_scene[0][1]
        scene_coords.firsttempbottomright = temp_coord_scene[0][2]
        scene_coords.firsttempbottomleft = temp_coord_scene[0][3]

        scene_coords.secondtempupleft = temp_coord_scene[1][0]
        scene_coords.secondtempupright = temp_coord_scene[1][1]
        scene_coords.secondtempbottomright = temp_coord_scene[1][2]
        scene_coords.secondtempbottomleft = temp_coord_scene[1][3]
        #centr Message
        scene_coords.firstcentrupleft = centrif_coord_scene[0][0]
        scene_coords.firstcentrupright = centrif_coord_scene[0][1]
        scene_coords.firstcentrbottomright = centrif_coord_scene[0][2]
        scene_coords.firstcentrbottomleft = centrif_coord_scene[0][3]

        scene_coords.secondcentrupleft = centrif_coord_scene[1][0]
        scene_coords.secondcentrupright = centrif_coord_scene[1][1]
        scene_coords.secondcentrbottomright = centrif_coord_scene[1][2]
        scene_coords.secondcentrbottomleft = centrif_coord_scene[1][3]

        scene_coords.thirdcentrupleft = centrif_coord_scene[2][0]
        scene_coords.thirdcentrupright = centrif_coord_scene[2][1]
        scene_coords.thirdcentrbottomright = centrif_coord_scene[2][2]
        scene_coords.thirdcentrbottomleft = centrif_coord_scene[2][3]

        scene_coords.fourthcentrupleft = centrif_coord_scene[3][0]
        scene_coords.fourthcentrupright = centrif_coord_scene[3][1]
        scene_coords.fourthcentrbottomright = centrif_coord_scene[3][2]
        scene_coords.fourthcentrbottomleft = centrif_coord_scene[3][3]
        #speed Message 
        scene_coords.speedupright = speed_coord_scene[0]
        scene_coords.speedupright = speed_coord_scene[1]
        scene_coords.speedbottomright = speed_coord_scene[2]
        scene_coords.speedbottomleft = speed_coord_scene[3]

        # SYMBOLS
        scene_coords.s1upleft = symbol_coord_scene[0][0]
        scene_coords.s1upright = symbol_coord_scene[0][1]
        scene_coords.s1bottomleft = symbol_coord_scene[0][3]
        scene_coords.s1bottomright = symbol_coord_scene[0][2]

        scene_coords.s2upleft = symbol_coord_scene[1][0]
        scene_coords.s2upright = symbol_coord_scene[1][1]
        scene_coords.s2bottomleft = symbol_coord_scene[1][3]
        scene_coords.s2bottomright = symbol_coord_scene[1][2]

        scene_coords.s3upleft = symbol_coord_scene[2][0]
        scene_coords.s3upright = symbol_coord_scene[2][1]
        scene_coords.s3bottomleft = symbol_coord_scene[2][3]
        scene_coords.s3bottomright = symbol_coord_scene[2][2]

        scene_coords.s4upleft = symbol_coord_scene[3][0]
        scene_coords.s4upright = symbol_coord_scene[3][1]
        scene_coords.s4bottomleft = symbol_coord_scene[3][3]
        scene_coords.s4bottomright = symbol_coord_scene[3][2]

        scene_coords.s5upleft = symbol_coord_scene[4][0]
        scene_coords.s5upright = symbol_coord_scene[4][1]
        scene_coords.s5bottomleft = symbol_coord_scene[4][3]
        scene_coords.s5bottomright = symbol_coord_scene[4][2]

        scene_coords.s6upleft = symbol_coord_scene[5][0]
        scene_coords.s6upright = symbol_coord_scene[5][1]
        scene_coords.s6bottomleft = symbol_coord_scene[5][3]
        scene_coords.s6bottomright = symbol_coord_scene[5][2]

        scene_coords.s7upleft = symbol_coord_scene[6][0]
        scene_coords.s7upright = symbol_coord_scene[6][1]
        scene_coords.s7bottomleft = symbol_coord_scene[6][3]
        scene_coords.s7bottomright = symbol_coord_scene[6][2]

        scene_coords.s8upleft = symbol_coord_scene[7][0]
        scene_coords.s8upright = symbol_coord_scene[7][1]
        scene_coords.s8bottomleft = symbol_coord_scene[7][3]
        scene_coords.s8bottomright = symbol_coord_scene[7][2]

        scene_coords.s9upleft = symbol_coord_scene[8][0]
        scene_coords.s9upright = symbol_coord_scene[8][1]
        scene_coords.s9bottomleft = symbol_coord_scene[8][3]
        scene_coords.s9bottomright = symbol_coord_scene[8][2]

        scene_coords.s10upleft = symbol_coord_scene[9][0]
        scene_coords.s10upright = symbol_coord_scene[9][1]
        scene_coords.s10bottomleft = symbol_coord_scene[9][3]
        scene_coords.s10bottomright = symbol_coord_scene[9][2]

        scene_coords.s11upleft = symbol_coord_scene[10][0]
        scene_coords.s11upright = symbol_coord_scene[10][1]
        scene_coords.s11bottomleft = symbol_coord_scene[10][3]
        scene_coords.s11bottomright = symbol_coord_scene[10][2]

        scene_coords.s12upleft = symbol_coord_scene[11][0]
        scene_coords.s12upright = symbol_coord_scene[11][1]
        scene_coords.s12bottomleft = symbol_coord_scene[11][3]
        scene_coords.s12bottomright = symbol_coord_scene[11][2]

        scene_coords.s13upleft = symbol_coord_scene[12][0]
        scene_coords.s13upright = symbol_coord_scene[12][1]
        scene_coords.s13bottomleft = symbol_coord_scene[12][3]
        scene_coords.s13bottomright = symbol_coord_scene[12][2]

        # Program and Options 
        scene_coords.prg1left = program_coord_scene[0][0]
        scene_coords.prg1right = program_coord_scene[0][1]
        scene_coords.prg2left = program_coord_scene[0][3]
        scene_coords.prg2right = program_coord_scene[0][2]

        scene_coords.prg2left = program_coord_scene[1][0]
        scene_coords.prg2right = program_coord_scene[1][1]
        scene_coords.prg3left = program_coord_scene[1][3]
        scene_coords.prg3right = program_coord_scene[1][2]

        scene_coords.prg3left = program_coord_scene[2][0]
        scene_coords.prg3right = program_coord_scene[2][1]
        scene_coords.prg4left = program_coord_scene[2][3]
        scene_coords.prg4right = program_coord_scene[2][2]

        scene_coords.prg4left = program_coord_scene[3][0]
        scene_coords.prg4right = program_coord_scene[3][1]
        scene_coords.prg5left = program_coord_scene[3][3]
        scene_coords.prg5right = program_coord_scene[3][2]

        scene_coords.prg5left = program_coord_scene[4][0]
        scene_coords.prg5right = program_coord_scene[4][1]
        scene_coords.prg6left = program_coord_scene[4][3]
        scene_coords.prg6right = program_coord_scene[4][2]

        scene_coords.prg6left = program_coord_scene[5][0]
        scene_coords.prg6right = program_coord_scene[5][1]
        scene_coords.prg7left = program_coord_scene[5][3]
        scene_coords.prg7right = program_coord_scene[5][2]

        scene_coords.prg7left = program_coord_scene[6][0]
        scene_coords.prg7right = program_coord_scene[6][1]
        scene_coords.prg8left = program_coord_scene[6][3]
        scene_coords.prg8right = program_coord_scene[6][2]

        scene_coords.prg8left = program_coord_scene[7][0]
        scene_coords.prg8right = program_coord_scene[7][1]
        scene_coords.prg9left = program_coord_scene[7][3]
        scene_coords.prg9right = program_coord_scene[7][2]

        scene_coords.prg9left = program_coord_scene[8][0]
        scene_coords.prg9right = program_coord_scene[8][1]
        scene_coords.prg10left = program_coord_scene[8][3]
        scene_coords.prg10right = program_coord_scene[8][2]

        scene_coords.prg10left = program_coord_scene[9][0]
        scene_coords.prg10right = program_coord_scene[9][1]
        scene_coords.prg11left = program_coord_scene[9][3]
        scene_coords.prg11right = program_coord_scene[9][2]       
        
        coord_pub.publish(scene_coords)
        #Visualize the video
        cv.namedWindow("scene image")
        cv.imshow("scene image",sceneRGBImg)
        cv.waitKey(3)
        rate.sleep()
    cv.release()
    cv.destroyAllWindows()
        









