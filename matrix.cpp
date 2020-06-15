#include <vector>
#include <iostream>
#include <cmath>


class Matrix{
  std::vector<std::vector<int>> matrix_int;  
  int rows, cols;
  public:
      Matrix()
      {
          
      }
      Matrix(const std::vector<std::vector<int> > &in_array):rows(in_array.size()), cols{in_array[0].size()}
      { 
        
          for (int i=0; i<rows;i++)
          {
            matrix_int.push_back(in_array[i]);
          }
      }
      void scalarMultiplication(int c)
      {
          for (int i=0; i<rows;i++)
          {
              for(int j=0; j<cols; j++)
              {
                  matrix_int[i][j] *= c;
              }
          }
      }
      std::vector<std::vector<int> > getMatrix()
      {
            return matrix_int;
      }
      //Burani duzeldersen 
      int getMatrixShape()
      {
            return array_int.size();
      }
      
      void displayMatrix()
      {
          for (int i=0; i<rows;i++)
          {
              for(int j=0; j<cols; j++)
              {
                  std::cout<<matrix_int[i][j]<<"\t";
              }
              std::cout<<std::endl;
          }
      }
};

class Vector{
  std::vector<int> array_int;
  int vec_length;
  public:
    Vector()
    {
        
    }
    Vector(std::vector<int> in_array):vec_length(in_array.size())
    { 
      for (int i = 0; i < vec_length; i++)
      {
          array_int.push_back(in_array[i]);
      }
    }
    void scalarMultiplication(const int c)
    {
      for (int i = 0; i < vec_length; i++)
      {
        array_int[i] *= c;
      }      
    }
   void vectorSum(Vector &v2)
    {
        if(this->getVectorLength() == v2.getVectorLength())
        {
            for (int i=0; i<vec_length; i++)
            {
              array_int[i]= array_int[i]+v2.getVector()[i];
            }
        }
        else
        {
            std::cout<<"Vector dimensions are not equal"<<std::endl;
        }
    }
    void setVector(const std::vector<int> &input)
    {   
        array_int.clear();
        for(int x : input)
        {
            array_int.push_back(x);
        }
    }
    std::vector<int> getVector()
    {
        return array_int;
    }
    
    int getVectorLength()
    {
        return array_int.size();
    }
    
    void displayVector()
    {
      for (int x : array_int)
      {
       std::cout<<x<<std::endl;
      }      
    }
};
// Sum function for 2 different vectors
void sum2vec(Vector v1, Vector v2)
{
   Vector v3;
   std::vector<int> v1_vec = v1.getVector();
   std::vector<int> v2_vec = v2.getVector();
   std::vector<int> new_vec;
   std::cout<<"v1 size "<<v1.getVectorLength()<<": v2 size "<<v2_vec.size()<<std::endl;
   if(v1.getVectorLength() == v2.getVectorLength())
   {
       for(int i=0; i<v1.getVectorLength(); i++)
       {
           new_vec.push_back(v1_vec[i]+v2_vec[i]);
       }
       v3.setVector(new_vec);
       v3.displayVector();
   }
   else
   {
       std::cout<<"Vector dimensions are not the same"<<std::endl;
   }
    
}

int main()
{ 
  std::vector<int> a = {5,1,2,6,6};
  Vector arr1(a);
  arr1.scalarMultiplication(3.5);
  arr1.displayVector();
  std::vector<int> b = {1,2,3,4,0};
  Vector arr2(b);
  sum2vec(arr1,arr2);
  
  std::vector<std::vector<int> > A_matrix ={{1,2,3},{4,5,6}};
  Matrix Mat(A_matrix);
  Mat.scalarMultiplication(4);
  Mat.displayMatrix();
  return 0;
}
