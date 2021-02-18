# UI_INTERPRETATION

UI_Interpretation is a ROS package used for:
 - User Interface interpretation of a specific domestic appliance using Deep Learning
 - Detection of 4 pieces of the appliance using Deep Learning 
 -- Knob 
-- Detergent Box 
-- User Interface 
-- Glass door
- Pose Estimation of the appliance using PnP estimation

# Requirements
  - python2.7
  - Tensorflow2 compatible with python2.7
  - opencv python 
  - numpy library in python 
  - transforms3d library in python
  - collections library in python 

### Charachteristics
This software has 2 main applications:
- UI interpretation 
- Object Detection

**Object Detection** finds 4 necessary coordinates to estimate pose of the appliance. 
It publishes pose attached to the knob with respect to base_link  under the topic *transformation_wrt_base*.
It also published the pose of the appliance with respect to depth optical frame of Tiago robot under the topic *transformation_wrt_depth_optical_frame*.
The former one can be used for robot manipulation on the washing machine. The latter is intended to be used for pcl registration in different ROS package named pcl_registration.
### To install and prepare your environment
1) Assuming you have python2.7, it is necessary to install tensorflow2 compatible with python2.7 to your computer if you don't have tensorflow2 already installed.

First, you should install virtual environment package in order to create specific python environment for tensorflow2.
```sh
$ python -m pip install virtualenv
```
After installing virtual environment, you can create specific tensorflow environment:
```sh
$ python -m virtualenv --system-site-packages ~/tensorflow2_python2.7/venv
```
Enter virtual environment of Tensorflow2.
```sh
$ source ~/PATH_TO_TENSORFLOW2_COMPATIBLE_WITH_PYTHON2.7/venv/bin/activate
```
NOTE: In case to quit out of virtual environment just type 
```sh
$ deactivate
```
Now you can install tensorflow2 into that environment.
```sh
$ pip install --upgrade tensorflow-gpu
```
While the virtual environment of tensorflow is active, you should install collections, opencv and transforms3d packages into the environment. 
```sh
$ python -m pip install collections python-opencv transforms3d
```
2) First copy ui_interpretation package to your ROS workspace.
3) Built the package 
```sh
$ cd YOUR_ROS_WORKSPACE
$ catkin_make
```


### Usage of Object Detection
1) Enter virtual environment of Tensorflow2 if it is not active.
```sh
$ source ~/PATH_TO_TENSORFLOW2_COMPATIBLE_WITH_PYTHON2.7/venv/bin/activate
```

2) Assuming you are in your ROS_WORKSPACE, source your ROS_WORKSPACE
```sh
$ source devel/setup.bash
```
3) Run *obj_detection.py* script
```sh
$ rosrun ui_interpretation obj_detection.py
```
