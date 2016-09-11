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
		//Creates a empty matrix given from a .txt file
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
		void printMatrix(){
			for (int i = 1; i <= rows; i++){
  				for (int j = 1; j <= cols; j++){
  					cout<< data[i][j]<<" ";
  				}
    			cout<<endl;
  			}
  			cout<<endl;

		}

		
	
};



vector< vector<double> > matrixMult(vector< vector<double> > &matrix1,vector< vector<double> > &matrix2,
 int rows1,int cols1,int rows2,int cols2){
	vector< vector<double> > result;
	createMatrix(result,rows1,cols2);
	for(int i=1;i<=rows1;i++){

		for(int j=1;j<=cols2;j++){
			result[i][j]=0;
			for(int k=1;k<=cols1;k++){
				result[i][j]=result[i][j]+ (matrix1[i][k]*matrix2[k][j]);
			}

		}
	}
	return result;
}


int main(int argc, char const *argv[])
{
	vector< vector<double> > matrix1,matrix2,result;

	Matrix A("1.txt");
	A.printMatrix();
	Matrix B("2.txt");
	B.printMatrix();

	return 0;
}