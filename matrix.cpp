#include <vector>
#include <iostream>
#include <cmath>


class Matrix{
  protected:
      std::vector<std::vector<int>> array_int;  
      int rows, cols;
  public:
      Matrix()
      {
          
      }
      Matrix(const std::vector<std::vector<int>> &in_array):rows(in_array.size()), cols{in_array[0].size()}
      { 
        
          for (int i=0; i<rows;i++)
          {
            array_int.push_back(in_array[i]);
          }
      }
      void scalarMultiplication(const int c)
      {
          for (int i=0; i<rows;i++)
          {
              for(int j=0; j<cols; j++)
              {
                  array_int[i][j] *= c;
              }
          }
      }
      void sum(Matrix &m2)
      {
        if(this->rows == m2.rows && this->cols == m2.cols)
        {
            for (int i=0; i<this->rows; i++)
            {
                for (int j=0; j<this->cols; j++)
                {
                    array_int[i][j] = array_int[i][j]+m2.getArray()[i][j];
                }
            }
        }
        else
        {
            std::cout<<"Dimensions are not equal"<<std::endl;
        }
      }
      void setArray(const std::vector<std::vector<int> > &input_matrix)
      {  
          rows = input_matrix.size();
          cols = input_matrix[0].size();
          for (int i=0; i<rows;i++)
          { 
              std::vector<int> array_row;
              for(int j=0; j<cols; j++)
              {
                  array_row.push_back(input_matrix[i][j]);
              }
              array_int.push_back(array_row);
          }
          
      }
      std::vector<std::vector<int>> getArray()
      {
          return array_int;
      }
      int getRows()
      {
          return rows; 
      }
      int getCols()
      {
          return cols;
      }
      
      void display()
      {
          for (int i=0; i<rows;i++)
          {
              for(int j=0; j<cols; j++)
              {
                  std::cout<<array_int[i][j]<<"\t";
              }
              std::cout<<std::endl;
          }
      }
};


class Vector: public Matrix
{
  int vec_length;
  int rows,cols;
  bool row_vector = false;
  public:
    Vector()
    {
        
    }
    Vector(const std::vector<std::vector<int>> &in_array):rows(in_array.size()), cols{in_array[0].size()}
    { 
      if(in_array.size()==1 && in_array[0].size()>1)
      {
          //raw vector
          for (int i=0; i<rows;i++)
          {
            array_int.push_back(in_array[i]);
          }
          vec_length = cols;
          row_vector = true;
      }
      else if(in_array.size()>1 && in_array[0].size()==1)
      {
         //column vector
         for (int i=0; i<rows;i++)
          {
            array_int.push_back(in_array[i]);
          }
          vec_length=rows;
          row_vector = false;
      }
      else
      {
          std::cout<<"It is not a vector";
      }
    }
    void scalarMultiplication(const int &c)
    {
          for (int i=0; i<rows;i++)
          {
              for(int j=0; j<cols; j++)
              {
                  array_int[i][j] *= c;
              }
          }
    }
    void sum(Vector &v2)
    {
        if(this->rows == v2.rows && this->cols == v2.cols)
        {
            for (int i=0; i<this->rows; i++)
            {
                for (int j=0; j<this->cols; j++)
                {
                    array_int[i][j] = array_int[i][j]+v2.getArray()[i][j];
                }
            }
        }
        else
        {
            std::cout<<"Dimensions are not equal"<<std::endl;
        }
    }
    void setArray(const std::vector<std::vector<int> > &input_matrix)
    {  
          rows = input_matrix.size();
          cols = input_matrix[0].size();
          std::vector<int> array_row;
          for (int i=0; i<rows;i++)
          { 
              
              for(int j=0; j<cols; j++)
              {
                  array_row.push_back(input_matrix[i][j]);
              }
              array_int.push_back(array_row);
              array_row.clear();
          }
    }
    int getVectorLength()
    {
        return vec_length;
    }
    int getRows()
    {
        return rows; 
    }
    int getCols()
    {
        return cols;
    }
    void display()
    {
        if(row_vector==true)
        {
           for(int j=0; j<cols; j++)
              {
                  std::cout<<array_int[0][j]<<"\t";
              }
              std::cout<<std::endl;
        }
        else
        {
           for(int j=0; j<rows; j++)
              {
                  std::cout<<array_int[j][0]<<"\n";
              }
        }
          
    }
};

//Dot product 
Vector dot(Matrix m1, Vector v1)
{   
    int scalar = 0;
    std::vector<std::vector<int>> m1_array = m1.getArray();
    std::vector<std::vector<int>> v1_array = v1.getArray();
    std::vector<int> vec_row;
    std::vector<std::vector<int>> vec;
    Vector vec_dot_result;
    if(m1.getCols() == v1.getRows())
    {
        for(int i=0; i<m1.getRows(); i++)
        {
            for(int j=0; j<m1.getCols(); j++)
            {
                scalar += m1_array[i][j]*v1_array[j][0];
            }
            
            vec_row.push_back(scalar);
            vec.push_back(vec_row);
            vec_row.clear();
        }
        vec_dot_result.setArray(vec);
        
    }
    else
    {
        std::cout<<"Matrix and vector dimensions are not suitable"<<std::endl;
    }
    return vec_dot_result; 
}
// Sum two different matrixes 
Matrix sum(Matrix m1, Matrix m2)
{
    Matrix m3;
    std::vector<std::vector<int>> m1_mat = m1.getArray();
    std::vector<std::vector<int>> m2_mat = m2.getArray();
    std::vector<std::vector<int>> sum_mat;
    
    int rows = m1.getRows();
    int cols = m1.getCols();
    
    if(m1.getRows()==m2.getRows() && m1.getCols()==m2.getCols())
    {   
        std::cout<<" Matrix dimensions are the same"<<std::endl;
        std::vector<int> matrix_row;
        for(int i=0; i<rows; i++)
        {
            for(int j=0; j<cols; j++)
            {  
               int sum =  m1_mat[i][j]+m2_mat[i][j];
               matrix_row.push_back(sum);
            }
            sum_mat.push_back(matrix_row);
            matrix_row.clear();
        }
        m3.setArray(sum_mat);
        
    }
    else
    {
        std::cout<<"Matrix shapes should be the same"<<"\n";
    }
    return m3;
}


int main()
{ 
  std::cout<<"-------- Vector operations --------"<<"\n";
  std::cout<<" Vector a: "<<"\n";
  std::vector<std::vector<int>> a = {{5,1}};
  Vector arr1(a);
  arr1.display();
  std::cout<<" Vector b: "<<"\n";
  std::vector<std::vector<int>> b = {{1,2}};
  Vector arr2(b);
  arr2.display();
  // Vector scalar multiplication
  std::cout<<"---- Scalar multiplication of the vector a by 3 ----"<<std::endl;
  arr1.scalarMultiplication(3);
  arr1.display();
  // Vector sum
  std::cout<<"---- Sum of two vector after scalar multiplication ----"<<std::endl;
  arr1.sum(arr2);
  arr1.display();

  std::cout<<"-------- Matrix operations --------"<<"\n";
  std::cout<<" Matrix a: "<<"\n";
  std::vector<std::vector<int>> a_1 = {{5,1,3},{3,4,5}};
  Matrix mat1(a_1);
  mat1.display();
  std::cout<<" Matrix b: "<<"\n";
  std::vector<std::vector<int>> b_1 = {{1,2,4},{4,6,5}};
  Matrix mat2(b_1);
  mat2.display();
  // Matrix scalar multiplication
  std::cout<<"---- Scalar multiplication of the matrix a by 3 ----"<<std::endl;
  mat1.scalarMultiplication(3);
  mat1.display();
  // Matrix sum 
  std::cout<<"------- Matrix sum -------"<<std::endl;
  Matrix new_mat = sum(mat1,mat2);
  new_mat.display();
  std::cout<<"------ Dot product -----------"<<std::endl;
  std::cout<<" Case: Dimensions are suitable "<<std::endl;
  std::vector<std::vector<int>> dot_mat1_array = {{5,4},{1,4}};
  Matrix dot_mat1(dot_mat1_array);
  std::vector<std::vector<int>> dot_vec1_array = {{15},{4}};
  Vector dot_vec1(dot_vec1_array);
  Vector new_vec1 = dot(dot_mat1,dot_vec1);
  new_vec1.display();
  std::cout<<" Case: Dimensions are not suitable "<<std::endl;
  std::vector<std::vector<int>> dot_mat2_array = {{10,4},{1,10}};
  Matrix dot_mat2(dot_mat2_array);
  std::vector<std::vector<int>> dot_vec2_array = {{1,2}};
  Vector dot_vec2(dot_vec2_array);
  Vector new_vec2 = dot(dot_mat2,dot_vec2);
  new_vec2.display();
  return 0;
}  
