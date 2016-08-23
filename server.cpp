#include <iostream>
#include <string>
#include <math.h>
#include <zmqpp/zmqpp.hpp>
#include <sstream>
using namespace std;
using namespace zmqpp;


//creates matrix m[cols][rows]
 vector< vector<int> > createMatrix(vector< vector<int> > matrix,int rows, int cols){

  
  matrix.resize(rows+1);

  for (int i = 1; i <= rows; i++) {
    matrix[i].resize(cols+1);
  }
return matrix;
}

/*
   int aux = 0;
  int count = 0;
  for(int i = 0; i < op1; i++){
    vector<int> x;
      for(int j = 0; j < op2; j++){
          stringstream ss;
          ss << m[count];
          ss >> aux;
          x.push_back(aux);
          count ++;
    }
    matrix1.push_back(x);x*/

 vector< vector<int> > fillMatrix(string matrix,int rows,int cols){
  vector< vector<int> > m;
  vector< vector<int> > new_matrix=createMatrix(m,rows,cols);
  int aux=0;
  int count=0;
for (int k = 0; k < matrix.size(); ++k)
{
  

  for (int i = 1; i <= rows; i++)
  {
    for (int j = 1; j <= cols; j++)
    {
      if (matrix[k]!=',')
      {
        //aqui falta algo para hacer que llene el vector de vectores correctamente
      
      cout << "Ingrese el numero de la posicion " << i << " " << j <<" "<<matrix[k]<< endl;
      //new_matrix[i][j]=stoi(matrix[k]);
      }
    }
  }
}
return new_matrix;


}

 void printMatrix(vector< vector<int> > matrix,int rows,int cols){

  for (int i = 1; i <= rows; i++)
  {
    for (int j = 1; j <= cols; j++)
    {
      
      cout<< matrix[i][j]<<" ";
    }
    cout<<endl;
  }




}

int main(int argc, char *argv[]) {
  // initialize the 0MQ context
  context ctx;

  // generate a pull socket
  socket s(ctx, socket_type::reply);

  // bind to the socket
  cout << "Binding " << endl;
  s.bind("tcp://*:4242");

  while (true) {
    // receive the message
    cout << "Receiving message..." << endl;
    message req;
    s.receive(req);

    string op,matrix1,matrix2;
    int rows1,cols1,rows2,cols2;
    rows1=cols1=rows2=cols2=0;

    req >> op >> rows1 >> cols1 >> rows2 >> cols2 >> matrix1 >> matrix2;

    vector< vector<int> > matriz=fillMatrix(matrix1,rows1,cols1);

    printMatrix(matriz,rows1,cols1);





    cout<<rows1<<endl;
    

    message rep;
    rep << rows1;
    s.send(rep);
    cout << "Sent: " << rows1+5 << endl;
  }
  cout << "Finished." << endl;
}

