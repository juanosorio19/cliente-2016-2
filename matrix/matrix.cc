#include <iostream>

#include <math.h>
#include <bits/stdc++.h>
using namespace std;

class Matrix
{

	private:
		 vector< vector<double> > data;
		 int rows;
		 int cols;
	public:
		Matrix(){};
		//Creates a matrix object given from a .txt file
		Matrix(const string & file){
			double iterator;           // file contains an undermined number of integer values
		    ifstream fin;     // declare stream variable name
			fin.open(file.c_str(),ios::in);    // open file
		   	fin >> iterator;  
		    rows=iterator;      
		    fin >> iterator; 
		    cols=iterator;
		    data.resize(rows+1);
  			for (int i = 1; i <= rows; i++){
  				data[i].resize(cols+1);
  			}
  			bool flag=true;
  			while ( flag==true )      //if not at end of file, continue reading numbers
  				{
  					for(int m=1;m<=rows;++m){
  						for(int n=1;n<=cols;n++){
  							fin >> iterator;
  							data[m][n]=iterator;
  						}
  					}
  					flag=false;
  				}

		};
		//constructor that creates an empty matrix of m*n 
		Matrix(const int &m,const int &n){
			rows=m;
			cols=n;
			data.resize(rows+1);
			for (int i = 1; i <= rows; i++){
				data[i].resize(cols+1);
			}

		};
		int getCols(){return cols;}
		int getRows(){return rows;}
		void printMatrix(){
			for (int i = 1; i <= rows; i++){
  				for (int j = 1; j <= cols; j++){
  					cout<< data[i][j]<<" ";
  				}
    			cout<<endl;
  			}
  			cout<<endl;

		}
		Matrix matrixMult(const Matrix & b){
			//Assertion missed, a.cols and b.rows have to be equals 
			Matrix result(this->rows,b.cols);

			for(int i=1;i<=this->rows;i++){
				for(int j=1;j<=b.cols;j++){
					result.data[i][j]=0;
					for(int k=1;k<=this->cols;k++){
						result.data[i][j]=result.data[i][j]+ (this->	data[i][k]*b.data[k][j]);
					}

				}
			}
			return result;
		}


		
	
};

int main(int argc, char const *argv[])
{
	vector< vector<double> > matrix1,matrix2,result;

	Matrix A("1.txt");
	A.printMatrix();
	Matrix B("2.txt");
	B.printMatrix();
	Matrix C=A.matrixMult(B);
	C.printMatrix();

	return 0;
}