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
		void setRow(int i,int j,double number){
			data[i][j]=number;
		}
		Matrix getCol(int col){
			Matrix result(this->rows,1);
			for(int i=1;i<=this->rows;i++){
				result.setData(i,1,(this->data[i][col]));
			}
			return result;
		}
		Matrix getRow(int row){
			Matrix result(1,this->cols);
			result.data[1]=this->data[row];
			/*
			for(int i=1;i<=this->rows;i++){
				result.setData(i,1,(this->data[i][col]));
			}*/
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

		Matrix matrixSum(Matrix &b){
			Matrix result(this->rows,this->cols);
			for(int i=1;i<=this->rows;i++){
				for(int j=1;j<=this->cols;j++){
					result.setData(i,j,(this->data[i][j] + b.getData(i,j) ) );
				}
			}
			return result;
		}

		void addToResult(Matrix & result,int r){
			
			if(r==1){
				for(int i=1;i<=this->rows;i++){
					for(int j=1;j<=this->cols;j++){
						result.setData(i,j,this->data[i][j]);
					}
				}
			}
			if(r==3){
				int a=result.getCols()/2+1;
				for(int i=1;i<=this->rows;i++){
					for(int j=1;j<=this->cols;j++){
						result.setData(a,j,this->data[i][j]);
					}
					a+=1;
				}
			}
			if(r==2){
				int a=1;
				for(int i=1;i<=this->rows;i++){
					int b=result.getCols()/2+1;

					for(int j=1;j<=this->cols;j++){
						result.setData(a,b,this->data[i][j]);
						b+=1;
					}
				a+=1;
				}
			}
			if(r==4){
				int a=result.getCols()/2+1;
				for(int i=1;i<=this->rows;i++){
					int b=result.getCols()/2+1;
					for(int j=1;j<=this->cols;j++){
						result.setData(a,b,this->data[i][j]);
						b+=1;
					}
				a+=1;
				}
			}
		}

		

		Matrix getSub(int r){
			Matrix tmp(this->rows/2,this->cols/2);
			//cout<<"a es de"<<this->rows<<" "<<this->cols<<endl;
			//cout<<"soy de "<<tmp.getRows()<<" "<<tmp.getCols()<<endl;
			if(r==1){
				for(int i=1;i<=this->rows/2;i++){
					for(int j=1;j<=this->cols/2;j++){
						//cout<<"hola estoy aqui\n";
						tmp.setData(i,j,this->data[i][j]);
					}
				}
				return tmp;
			}
			if(r==3){
				int tmpo=1;
				for(int i=this->rows/2+1;i<=this->rows;i++){
					for(int j=1;j<=this->cols/2;j++){
						//cout<<"escribo en "<<i<<" "<<j<<endl;
						tmp.setData(tmpo,j,this->data[i][j]);
					}
					tmpo+=1;
				}
				return tmp;
			}
			if(r==2){
				for(int i=1;i<=this->rows/2;i++){
					int tmpo=1;
					for(int j=this->cols/2;j<this->cols;j++){
						//cout<<"escribo en "<<i<<" "<<tmpo<<endl;
						//cout<<"escribo de "<<i<<" "<<j+1<<endl;
						tmp.setData(i,tmpo,this->data[i][j+1]);
						tmpo+=1;
					}
				}
				return tmp;
			}
			if(r==4){
				int tmpo=1;
				for(int i=this->rows/2+1;i<=this->rows;i++){
					int tmpo_2=1;
					for(int j=this->cols/2+1;j<=this->cols;j++){
						//cout<<"escribo en "<<tmpo<<" "<<tmpo_2<<endl;
						tmp.setData(tmpo,tmpo_2,this->data[i][j]);
						tmpo_2+=1;
					}
				tmpo+=1;
				}
				return tmp;
			}
		}
		
};
// Matrix multiplication function
void mult(const Matrix &a,const Matrix &b,Matrix &result,const int index,vector<bool> &threadState,const int count){
			//Assertion missed, this->cols and b.rows have to be equals
	if(!threadState[count]){
			cout<<count<<endl;
			for(int i=1;i<=a.getRows();i++){
				for(int j=1;j<=b.getCols();j++){
					
					for(int k=1;k<=b.getRows();k++){
						//cout<<"escribo posicion "<<index<<" "<<j<<endl;
						result.setData(index,j,( result.getData(index,j)+ (a.getData(i,k) * b.getData(k,j)) ));
					}
					

				}
			}
			threadState[count]=true; //put a thread ready
		}  
	}

Matrix threadMult(Matrix &a,Matrix &b){
	//Assertion missed, this->cols and b.rows have to be equals
	Matrix result(a.getRows(),b.getCols());
	
	//Get the number of CPUs in linux
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	vector< future <void> > threads(numCPU-1);
	vector< bool> threadState(numCPU-1,true);
	int count=0;
	
	for(int j=1;j<=b.getRows();j++){
		Matrix temporal=a.getRow(j);
		//temporal.printMatrix();
		//if (count >= numCPU-1) count = 0;
		
		if (count >= numCPU-1) count = 0;
	    if (threadState[count]) {
	    	//	cout<<"estado valido numero :"<<count<<endl;
	      	threadState[count]=false;	
	      	threads[count] =async(launch::async, mult,ref(temporal),ref(b),ref(result),j,ref(threadState),count);
	      	//result.printMatrix();
	      	//threads[count].get();
	      	
	      
	      	} else{
	      		threads[count].get();
	      		threadState[count]=false;	
	      		threads[count] =async(launch::async, mult,ref(temporal),ref(b),ref(result),j,ref(threadState),count);

	      	}
	 
    	count++;



	}
	for(int i=0;i<=threads.size();i++){
		if(threads[i].valid())
		threads[i].get();

	}

	return result;


}

// Simple and sequential multmatrix method
Matrix matrixMult(const Matrix &a, const Matrix & b,Matrix &result){
	//Assertion missed, this->cols and b.rows have to be equals 
	
	for(int i=1;i<=a.getRows();i++){
		for(int j=1;j<=b.getCols();j++){
			//result.setData(i,j,0);
			for(int k=1;k<=a.getCols();k++){
				result.setData(i,j,( result.getData(i,j) + ( a.getData(i,k)*b.getData(k,j) )));
			}

		}
	}
	return result;
}

void blockMult(Matrix &A,Matrix &B,Matrix &result){
	if(A.getCols()==2){
		matrixMult(A,B,result);
				
	}else{
		Matrix C(A.getCols()/2,A.getRows()/2),D(A.getCols()/2,A.getRows()/2),
		E(A.getCols()/2,A.getRows()/2),F(A.getCols()/2,A.getRows()/2),
		G(A.getCols()/2,A.getRows()/2),H(A.getCols()/2,A.getRows()/2),
		I(A.getCols()/2,A.getRows()/2),J(A.getCols()/2,A.getRows()/2);
		//subregion results
		Matrix CG(A.getCols()/2,A.getRows()/2),CH(A.getCols()/2,A.getRows()/2),
		EG(A.getCols()/2,A.getRows()/2),EH(A.getCols()/2,A.getRows()/2),
		DI(A.getCols()/2,A.getRows()/2),DJ(A.getCols()/2,A.getRows()/2),
		FI(A.getCols()/2,A.getRows()/2),FJ(A.getCols()/2,A.getRows()/2);

		Matrix B1(A.getCols()/2,A.getRows()/2),B2(A.getCols()/2,A.getRows()/2),
		B3(A.getCols()/2,A.getRows()/2),B4(A.getCols()/2,A.getRows()/2);
		
		C=A.getSub(1);
		D=A.getSub(2);
		E=A.getSub(3);
		F=A.getSub(4);
		G=B.getSub(1);
		H=B.getSub(2);
		I=B.getSub(3);
		J=B.getSub(4);

		blockMult(C,G,CG);
		blockMult(C,H,CH);
		blockMult(E,H,EH);
		blockMult(E,G,EG);

		blockMult(D,I,DI);
		blockMult(D,J,DJ);
		blockMult(F,J,FJ);
		blockMult(F,I,FI);

		B1=CG.matrixSum(DI);
		B2=CH.matrixSum(DJ);
		B3=EG.matrixSum(FI);
		B4=EH.matrixSum(FJ);

		B1.addToResult(result,1);
		B2.addToResult(result,2);
		B3.addToResult(result,3);
		B4.addToResult(result,4);
		
	}
}

int main(int argc, char const *argv[])
{
	Matrix A("exam.txt");
	//A.printMatrix();
	Matrix B("exam.txt");
	//B.printMatrix();
/*
	Matrix C(16,16);
	Matrix P(4,4);
	Matrix result(16,16);

*/
	Matrix P(100,100);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	Matrix C=threadMult(A,B);
	//Matrix C=A.getRow(1);;
	
	
	//matrixMult(A,B,P);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    
    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    matrixMult(A,B,P);
   // blockMult(A,B,result);
    //Matrix P=A.getCol(1);
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>( t4 - t3 ).count();


    //result.printMatrix();
   	C.printMatrix();
    //A.printMatrix();
    //P=A.getRow(1);
    //P.printMatrix();
    //z.printMatrix();
	cout << "duration was rows: "<<duration<<endl;
	//P.printMatrix();
    cout << "duration was cols : "<<duration1<<endl;

	return 0;
}