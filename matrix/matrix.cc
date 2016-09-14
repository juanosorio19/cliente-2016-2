#include <bits/stdc++.h>
#include <unistd.h>
#include <thread>
using namespace std;

class Matrix
{

	private:
		 vector< vector<double> > data;
		 int rows;
		 int cols;
	public:
		Matrix(){};
		//constructor that creates an empty matrix of m*n 
		Matrix(const int &m,const int &n){
			rows=m;
			cols=n;
			data.resize(rows+1);
			for (int i = 1; i <= rows; i++){
				data[i].resize(cols+1);
			}

		};
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
		const int &getCols()const{return cols;}
		const int &getRows()const{return rows;}
		const double &getData(int i,int j)const{return data[i][j];}
		void setData(int i,int j,double number){
			data[i][j]=number;
		}
		Matrix getCol(int col){
			Matrix result(this->rows,1);
			
			for(int i=1;i<=this->rows;i++){
				result.setData(i,1,(this->data[i][col]));
			}
			return result;
		}
		void printMatrix(){
			for (int i = 1; i <= rows; i++){
  				for (int j = 1; j <= cols; j++){
  					cout<< data[i][j]<<" ";
  				}
    			cout<<endl;
  			}
  			cout<<endl;

		}
		// Simple and sequential mult matrix (no threads)
		Matrix matrixMult(const Matrix & b){
			//Assertion missed, this->cols and b.rows have to be equals 
			Matrix result(this->rows,b.cols);

			for(int i=1;i<=this->rows;i++){
				for(int j=1;j<=b.cols;j++){
					result.data[i][j]=0;
					for(int k=1;k<=this->cols;k++){
						result.data[i][j]=result.data[i][j]+ (this->data[i][k]*b.data[k][j]);
					}

				}
			}
			return result;
		}
		void mult(Matrix &b,Matrix &result,const int &index){
			//Assertion missed, this->cols and b.rows have to be equals 
			

			for(int i=1;i<=this->rows;i++){
				for(int j=1;j<=b.cols;j++){
					result.data[i][index]=0;
					for(int k=1;k<=this->cols;k++){
						result.data[i][index]=result.data[i][index]+ (this->data[i][k]*b.data[k][j]);
					}

				}
			}
			
		}

	
};

Matrix threadMult( Matrix &a, Matrix &b){
	//Assertion missed, this->cols and b.rows have to be equals
	Matrix result(a.getRows(),b.getCols());
	//for(int i=1;i<=a.getRows();i++){
		for(int j=1;j<=b.getCols();j++){
			//result.setData(i,j,0);//initialize the pos i,j in 0
			Matrix temporal=b.getCol(j);
			a.mult(temporal,result,j);
			cout<<"asi vamos \n";
			result.printMatrix();
			

		}
//	}
	return result;


}

int main(int argc, char const *argv[])
{
	Matrix A("1.txt");
	A.printMatrix();
	Matrix B("2.txt");
	B.printMatrix();
	Matrix C=threadMult(A,B);
	C.printMatrix();
	//Get the number of CPUs in linux
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	cout<<"number is "<<numCPU<<endl;
	return 0;
}