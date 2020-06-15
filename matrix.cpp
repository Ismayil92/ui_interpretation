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
      Matrix(const std::vector<std::vector<int> > &in_array):rows(in_array.size()), cols{in_array[0].size()}
      { 
        
          for (int i=0; i<rows;i++)
          {
            array_int.push_back(in_array[i]);
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
            std::cout<<"Vector dimensions are not equal"<<std::endl;
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
            std::cout<<"Vector dimensions are not equal"<<std::endl;
        }
    }
    
    int getVectorLength()
    {
        return vec_length;
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
        std::cout<<"Matrix dimensions are the same"<<std::endl;
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

/* Sum function for 2 different vectors
Vector sum2vec(Vector v1, Vector v2)
{
   Vector v3;
   std::vector<int> v1_vec = v1.getVector();
   std::vector<int> v2_vec = v2.getVector();
   std::vector<int> new_vec;
   
   if(v1.getVectorLength() == v2.getVectorLength())
   {
       for(int i=0; i<v1.getVectorLength(); i++)
       {
           new_vec.push_back(v1_vec[i]+v2_vec[i]);
       }
       v3.setVector(new_vec);
   }
   else
   {
       std::cout<<"Vector dimensions are not the same"<<std::endl;
   }
   return v3;
}
*/
int main()
{ 
  std::vector<std::vector<int> > a = {{5,4},{1,4}};
  Matrix arr1(a);
  arr1.scalarMultiplication(3);
  arr1.display();
  std::vector<std::vector<int> > b = {{1,4},{2,4}};
  Matrix arr2(b);
  arr2.display();
  arr1.sum(arr2);
  arr1.display();
  
  Matrix new_matrix = sum(arr1,arr2);
  new_matrix.display();

  return 0;
}  

