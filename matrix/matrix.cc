#include <bits/stdc++.h>

using namespace std;

 void printMatrix(const vector< vector<double> > &matrix,int rows,int cols){

  for (int i = 1; i <= rows; i++){
  	for (int j = 1; j <= cols; j++){
  		cout<< matrix[i][j]<<" ";
  	}
    cout<<endl;
  }
 }

void createMatrix (vector< vector<double> > &matrix, int rows, int cols)
{
  matrix.resize(rows+1);
  for (int i = 1; i <= rows; i++)
  {
    matrix[i].resize(cols+1);
  }
  
}

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
	createMatrix(matrix1,2,2);
	createMatrix(matrix2,2,2);
	matrix1[1][1]=1;
	matrix1[1][2]=2;
	matrix1[2][1]=3;
	matrix1[2][2]=4;

	matrix2[1][1]=5;
	matrix2[1][2]=6;
	matrix2[2][1]=7;
	matrix2[2][2]=8;
	printMatrix(matrix1,2,2);
	printMatrix(matrix2,2,2);

	result=matrixMult(matrix1,matrix2,2,2,2,2);
	printMatrix(result,2,2);




	
	return 0;
}