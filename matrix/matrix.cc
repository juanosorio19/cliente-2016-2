#include <bits/stdc++.h>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;


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
		// Simple and sequential multmatrix method
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


	
};
// Matrix multiplication function
void mult(Matrix &a,Matrix &b,Matrix &result,const int &index,vector<bool> &threadState,int count){
			//Assertion missed, this->cols and b.rows have to be equals 
			
			for(int i=1;i<=a.getRows();i++){
				for(int j=1;j<=b.getCols();j++){
					result.setData(i,index,0);
					for(int k=1;k<=a.getCols();k++){
						//result.data[i][index]=result.data[i][index]+ (this->data[i][k]*b.data[k][j]);
						result.setData(i,index,( result.getData(i,index)+ (a.getData(i,k) * b.getData(k,j)) ));
					}

				}
			}
			threadState[count]=false;
		}  

Matrix threadMult(Matrix &a, Matrix &b){
	//Assertion missed, this->cols and b.rows have to be equals
	Matrix result(a.getRows(),b.getCols());
	
	//Get the number of CPUs in linux
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	vector< future <void> > threads(numCPU-1);
	vector< bool> threadState(numCPU-1,false);
	int count=0;
			
	for(int j=1;j<=b.getCols();j++){
		Matrix temporal=b.getCol(j);

    	if (count >= numCPU-1) count = 0;
    	if (!threadState[count]) {
    	//	cout<<"estado valido numero :"<<count<<endl;
      	threadState[count]=true;	
      	threads[count] =async(launch::async, mult,ref(a),ref(temporal),ref(result),ref(j),ref(threadState),count);
      	threads[count].get();
      	count++;
    	}
    	
  	
    	//async(launch::async, mult,ref(a),ref(temporal),ref(result),ref(j));
		//mult(a,temporal,result,j); //making a* b.col(j)
		//threads[j]= thread (mult,ref(a),ref(temporal),ref(result),ref(j)); //making a* b.col(j)
		//cout<<"asi vamos \n";
		//result.printMatrix();
		//threads[j].join();
	}
	cout<<"aqui estoy"<<endl;
/*	for(int i=1;i<=numCPU;++i){
		if (threads[count].wait_for(std::chrono::seconds(0))!= future_status::ready ) {
			threads[i].wait(); 
			cout<<"estado finalizado numero :"<<i<<endl;
		}
	}*/	

	return result;


}

int main(int argc, char const *argv[])
{
	Matrix A("1.txt");
	//A.printMatrix();
	Matrix B("1.txt");
	//B.printMatrix();
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	Matrix C=threadMult(A,B);
	//A.matrixMult(B);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    //C.printMatrix();
    cout << "duration was : "<<duration<<endl;

	return 0;
}