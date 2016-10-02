#include <bits/stdc++.h>
#include <atomic> 
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector> 
using namespace std;


#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class join_threads {
  std::vector<std::thread> &threads;

public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
     //std::cerr << "destructing joiner\n";
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable())
        threads[i].join();
    }
  }
};

template <typename T> class threadsafe_queue {
private:
  mutable std::mutex mut;
  std::queue<T> data_queue;
  std::condition_variable data_cond;

public:
  threadsafe_queue() {}
  void push(T data) {
    std::lock_guard<std::mutex> lk(mut);
    data_queue.push(std::move(data));
    data_cond.notify_one();
  }
  void wait_and_pop(T &value) {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    value = std::move(data_queue.front());
    data_queue.pop();
  }
  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));

    data_queue.pop();
    return res;
  }
  bool try_pop(T &value) {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty())
      return false;
    value = std::move(data_queue.front());
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty())
      return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
    data_queue.pop();
    return res;
  }
  bool empty() const {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
  }
};

class thread_pool {
  std::atomic_bool done;
  threadsafe_queue<std::function<void()>> work_queue;
  std::vector<std::thread> threads;
  join_threads *joiner;
  void worker_thread() {
    while (!done && !work_queue.empty()) {
      std::function<void()> task;
      if (work_queue.try_pop(task)) {
        task();
      } else {
        cout<<"ESTOY EN EL ELSE\n";
        std::this_thread::yield();
      }
    }
  }

public:
  thread_pool() : done(false), joiner(new join_threads(threads)) {
    // joiner(new join_threads(threads));
    unsigned const thread_count = std::thread::hardware_concurrency();
    try {
      for (unsigned i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread(&thread_pool::worker_thread, this));
      }
    } catch (...) {
      cout<<"ESTOY EN EL CATCH\n";
      done = true;
      throw;
    }
  }
  ~thread_pool() {
    joiner->~join_threads();
    done = true;
     std::string s("Destructing pool ");
     s += std::to_string(work_queue.empty());
     s += '\n';
     std::cerr << s<<endl;
  }
  template <typename FunctionType> void submit(FunctionType f) {
    work_queue.push(std::function<void()>(f));
       // std::cerr << std::this_thread::get_id() << std::endl;
  }
};


template <typename T>
class SparseMatrix {

private:

  int rows;
  int cols;
  vector<T> val;
  vector<int> colInd;
  vector<int> rowPtr;

  public:

  SparseMatrix(int r, int c) : rows(r), cols(c), rowPtr(r + 1,0) {}
  SparseMatrix(){}
  int getRows()const{return rows;}
  int getCols()const{return cols;}
  vector<T> getVal(){return val; }
  void printVal(){
    vector<int> cval=getVal();
    for(int i=0;i<=cval.size()-1;i++){
      cout<<cval[i]<<" ";
    
    }
  cout<<endl;
  

  }
  
  T get(int r, int c)const {
    //cout<<"Rows "<<r<<endl;
    //cout<<"Cols "<<c<<endl<<endl;
    assert(r>=0 and r<rows and c>=0 and c<cols);
    // Retorna el elemento que hay en la posici ́on (r,c)
    //asercion= r,c menores o iguales que rows,cols
    int rowNumber= rowPtr[r+1]-rowPtr[r]; // se cuantos elementos hay en la fila r
    int start= rowPtr[r]; // se cuantos elementos hay al empezar la fila r
    for(int i=0;i<rowNumber;i++){
      if(colInd[i+start]==c) return val[i+start]; // tengo el elemento en la posicion r,c
    }
    return 0;  //element(r,c)=0
  }
  //retorna un vector columna (c)
  vector<T> getCol(int c)const{
    vector<T> result(this->rows,0);
    for(int i=0;i<this-> rows;i++){
      if(get(i,c)!=0){
       result[i]=get(i,c);
      }

    }
    return result;
  }


  void set(const T value,const int r,const int c){
    assert(r>=0 and r<rows and c>=0 and c<cols);
    //mutex mtx;
    //mtx.lock();
  
    int rn=rowPtr[r+1]-rowPtr[r]; // cuantos elementos hay en fila r
    int start=rowPtr[r];
    
    if(get(r,c)!=0){
      for(int i=0;i<rn;i++){
        if(colInd[i+start]==c) val[i+start]=value; // tengo el elemento en la posicion r,c
        
      }
      return;
    }else if(rn==0){ // es el primer valor de la fila r
     val.emplace(val.begin()+start,value);
     colInd.emplace(colInd.begin()+start,c);
   
    } else{
      for(int i=0;i<rn;i++){
        if(c<colInd[rowPtr[r]+i]){
          val.emplace(val.begin()+start+i,value);
          colInd.emplace(colInd.begin()+start+i,c);
          for(int j=r+1;j<rowPtr.size();j++){ //redimensiono rowPtr desde r+1
            rowPtr[j]+=1;
          }
          return;
        }
        
      }
      val.emplace(val.begin()+start+rn,value);
      colInd.emplace(colInd.begin()+start+rn,c);

    }
    for(int j=r+1;j<rowPtr.size();j++){ //redimensiono rowPtr desde r+1
          rowPtr[j]+=1;
    }
    //mtx.unlock();

  }
// MULTIPLICACION DE MATRICES SECUENCIAL, PREGUNTA 1(2)
  void mult(SparseMatrix<T>& b,SparseMatrix<T> &result) {
  // Multiplica esta matriz con la matriz b
    int p=1;
    for(int i=0;i<rows;i++){
      for(int j=0;j<b.getCols();j++){
        int val=0;
        
        for(int k=0;k<b.getCols();k++){
          if((get(i,k)and b.get(k,j) ) !=0 ){
            result.set(result.get(i,j)+(get(i,k)*b.get(k,j)),i,j);
            p+=1;
            
          }

        }
      }
    }
  }


};/*

template <typename T>
SparseMatrix<T> multConcurrency(const SparseMatrix<T> &m1,
                                const SparseMatrix<T> &m2) {

  // Check
  assert(m1.getNumCols() == m2.getNumRows());

  thread_pool *pool = new thread_pool();
  vector<SparseMatrix<T>> results(m1.getNumRows(), {1, m2.getNumCols()});
  SparseMatrix<T> result(m1.getNumRows(), m2.getNumCols());

  for (int i = 0; i < m1.getNumRows(); i++) {
    auto func = [&m1, &m2, i, &results]() { multCol(m1, m2, i, results); };
    pool->submit(func);
  }

  delete pool;

  for (int i = 0; i < results.size(); i++) {
    for (int j = 0; j < results[i].getNumCols(); j++) {
      result.set(results[i].get(0, j), i, j);
    }
  }

  return result;
}


*/


























void sparseSum(SparseMatrix<int>& a, SparseMatrix<int>& b,SparseMatrix<int> &result){
  for(int i=0;i<a.getRows();i++){
    for(int j=0;j<b.getCols();j++){
      //result[i][j]=a[i][j]+b[i][j];
      result.set(a.get(i,j)+b.get(i,j),i,j);
    }
  }

}
void sparseSum(const vector< SparseMatrix<int> > & matrices,SparseMatrix<int> &result){
  for(int i=0;i<matrices[0].getRows();i++){
    for(int j=0;j<matrices[0].getCols();j++){
      int temporal=0;
      for(int k=0;k<matrices.size();k++){
        temporal+=matrices[k].get(i,j);
        //result.set(a.get(i,j)+b.get(i,j),i,j);

      }
    result.set(temporal,i,j);
    }
  }

}

void concurrentMult(const SparseMatrix<int>& a,const vector<int> b,SparseMatrix<int> &result,const int col,
  vector< SparseMatrix<int> > & matrices) {
  //cout<<"entré con "<<col<<endl;
  //std::cerr << std::this_thread::get_id() << std::endl;
  // Multiplica matriz a *columna
  
  for(int i=0;i<a.getRows();i++){
    for(int j=0;j<1 ;j++){
      int temporal=0;
      for(int k=0;k<b.size();k++){
        if((a.get(i,k)and b[k] ) !=0 ){
          temporal+=a.get(i,k)*b[k];
        }
      }
      //cout<<"escribo un "<<temporal<<" pos "<<i<<" "<<col<<endl;
      matrices[col].set(temporal,i,col);
      
      //result.set(temporal,i,col);
      //cerr<<"Escribí\n";
      //cout<<"AQUI ESTOY\n"; 
      //matrices[col].printVal();
      //cout<<"estoy en pos "<<i<<" "<<col<<endl;
      }
    }
  //matrices[col].printVal();
  


}
void mult(const SparseMatrix<int>& a,const SparseMatrix<int>& b,SparseMatrix<int> &result) {

  // Multiplica matriz a con la matriz b
  vector< SparseMatrix<int> > matrices(b.getCols(),SparseMatrix <int>(a.getRows(),b.getCols()));  
  
    //thread_pool pool;
    thread_pool *pool = new thread_pool();
    
    for(int j=0;j<b.getCols();j++){
      //cout<<"mando col "<<j<<endl;
      vector<int>temporal=a.getCol(j);
      

      //for(int i=0;i<temporal.size();i++){cout<<temporal[i]<<" "<<endl;}
      //cout<<"pase la prueba\n";
      auto w = [&a, temporal, &result,j,&matrices]() { concurrentMult(a, temporal, result, j,matrices); };
      //concurrentMult(a,temporal,result,j,matrices);
      
      //cerr<<"MANDO TAREA "<<j<<endl;
      //pool.submit(w);
      pool->submit(w);

    }
    delete pool;

    
   
  
  sparseSum(matrices,result);

  
   
}

int main(int argc, char const *argv[]) {
  SparseMatrix<int> a(3,3);


  a.set(1,0,0);
  a.set(9,2,2);
  a.set(6,1,2);
  a.set(4,1,0);
  a.set(2,0,1);
  a.set(8,2,1);
  a.set(3,0,2);
  a.set(5,1,1);
  a.set(7,2,0);
  SparseMatrix<int> ar(3,3);


 /* ar.set(1,0,0);
  ar.set(2,0,1);
  ar.set(3,0,2);
  ar.set(4,1,0);
  ar.set(5,1,1);
  ar.set(6,1,2);
  ar.set(7,2,0);
  ar.set(8,2,1);*/
  //ar.printVal();
  
  
  //a.printVal();
  SparseMatrix<int> c(3,3);
  SparseMatrix<int> result(3,3);
  mult(a,a,c);


  //sparseSum(a,a,c);
  cout<<"RESULTADO\n";
  c.printVal();


  return 0;
}




