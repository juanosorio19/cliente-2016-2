#include <atomic>
#include <bits/stdc++.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;
namespace ublas = boost::numeric::ublas;

using SM = ublas::compressed_matrix<int>;

class join_threads {
  std::vector<std::thread> &threads;

public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
    // std::cerr << "destructing joiner\n";
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
    while (!done or !work_queue.empty()) {
      // cout << "working" << endl;
      std::function<void()> task;
      if (work_queue.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield();
      }
    }
  }

public:
  thread_pool() : done(false), joiner(new join_threads(threads)) {
    // joiner(new join_threads(threads));
    unsigned const thread_count = 1; // std::thread::hardware_concurrency();
    try {
      for (unsigned i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread(&thread_pool::worker_thread, this));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }
  ~thread_pool() {
    // joiner->~join_threads();
    done = true;
    for (auto &thread : threads) {
      if (thread.joinable())
        thread.join();
    }
    /*std::string s("Destructing pool ");
    s += std::to_string(work_queue.empty());
    s += '\n';
    std::cerr << s << endl;*/
  }
  template <typename FunctionType> void submit(FunctionType f) {
    work_queue.push(std::function<void()>(f));
    // std::cerr << std::this_thread::get_id() << std::endl;
  }
};

template <typename T> class SparseMatrix {
private:
  int rows;
  int cols;
  vector<T> val;
  vector<int> colInd;
  vector<int> rowPtr;

public:
  SparseMatrix(int r, int c) : rows(r), cols(c), rowPtr(r + 1, 0) {}
  SparseMatrix(int r, int c, int nz) : rows(r), cols(c), rowPtr(r + 1, 0) {
    val.reserve(nz);
    colInd.reserve(nz);
  }
  SparseMatrix() {}
  int getRows() const { return rows; }
  int getCols() const { return cols; }
  vector<T> getVal() { return val; }
  void printVal() {
    vector<int> cval = getVal();
    for (int i = 0; i <= cval.size() - 1; i++) {
      cout << cval[i] << " ";
    }
    cout << endl;
  }

  T get(int r, int c) const {
    // cout<<"Rows "<<r<<endl;
    // cout<<"Cols "<<c<<endl<<endl;
    assert(r >= 0 and r < rows and c >= 0 and c < cols);
    // Retorna el elemento que hay en la posici ́on (r,c)
    // asercion= r,c menores o iguales que rows,cols
    int rowNumber =
        rowPtr[r + 1] - rowPtr[r]; // se cuantos elementos hay en la fila r
    int start = rowPtr[r]; // se cuantos elementos hay al empezar la fila r
    for (int i = 0; i < rowNumber; i++) {
      if (colInd[i + start] == c)
        return val[i + start]; // tengo el elemento en la posicion r,c
    }
    return 0; // element(r,c)=0
  }
  // retorna un vector columna (c)
  vector<T> getCol(int c) const {
    vector<T> result(this->rows, 0);
    for (int i = 0; i < this->rows; i++) {
      if (get(i, c) != 0) {
        result[i] = get(i, c);
      }
    }
    return result;
  }

  void set(const T value, const int r, const int c) {
    assert(r >= 0 and r < rows and c >= 0 and c < cols);
    // mutex mtx;
    // mtx.lock();

    int rn = rowPtr[r + 1] - rowPtr[r]; // cuantos elementos hay en fila r
    int start = rowPtr[r];

    if (get(r, c) != 0) {
      for (int i = 0; i < rn; i++) {
        if (colInd[i + start] == c)
          val[i + start] = value; // tengo el elemento en la posicion r,c
      }
      return;
    } else if (rn == 0) { // es el primer valor de la fila r
      val.emplace(val.begin() + start, value);
      colInd.emplace(colInd.begin() + start, c);

    } else {
      for (int i = 0; i < rn; i++) {
        if (c < colInd[rowPtr[r] + i]) {
          val.emplace(val.begin() + start + i, value);
          colInd.emplace(colInd.begin() + start + i, c);
          for (int j = r + 1; j < rowPtr.size();
               j++) { // redimensiono rowPtr desde r+1
            rowPtr[j] += 1;
          }
          return;
        }
      }
      val.emplace(val.begin() + start + rn, value);
      colInd.emplace(colInd.begin() + start + rn, c);
    }
    for (int j = r + 1; j < rowPtr.size();
         j++) { // redimensiono rowPtr desde r+1
      rowPtr[j] += 1;
    }
    // mtx.unlock();
  }
  // MULTIPLICACION DE MATRICES SECUENCIAL, PREGUNTA 1(2)
  void mult(SparseMatrix<T> &b, SparseMatrix<T> &result) {
    // Multiplica esta matriz con la matriz b
    int p = 1;
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < b.getCols(); j++) {
        int val = 0;

        for (int k = 0; k < b.getCols(); k++) {
          if ((get(i, k) and b.get(k, j)) != 0) {
            result.set(result.get(i, j) + (get(i, k) * b.get(k, j)), i, j);
            p += 1;
          }
        }
      }
    }
  }

}; /*

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

void sparseSum(SparseMatrix<int> &a, SparseMatrix<int> &b,
               SparseMatrix<int> &result) {
  for (int i = 0; i < a.getRows(); i++) {
    for (int j = 0; j < b.getCols(); j++) {
      // result[i][j]=a[i][j]+b[i][j];
      result.set(a.get(i, j) + b.get(i, j), i, j);
    }
  }
}
void sparseSum(const vector<SparseMatrix<int>> &matrices,
               SparseMatrix<int> &result) {
  for (int i = 0; i < matrices[0].getRows(); i++) {
    for (int j = 0; j < matrices[0].getCols(); j++) {
      int temporal = 0;
      for (int k = 0; k < matrices.size(); k++) {
        temporal += matrices[k].get(i, j);
        // result.set(a.get(i,j)+b.get(i,j),i,j);
      }
      result.set(temporal, i, j);
    }
  }
}

void concurrentMult(const SM &a, const int col, SM &result, const SM &b) {
  // cerr << "entré con " << col << endl;
  auto c = row(b, col);

  for (auto cit = c.begin(); cit != c.end(); ++cit) {
    for (auto it1 = a.begin1(); it1 != a.end1(); ++it1) {
      int temporal =0;
      for (auto it2 = it1.begin(); it2 != it1.end(); ++it2) {
        // cout << "(" << cit.index() << "," << *cit << ")" << endl;

        if (it2.index2() == cit.index()) {
          cout << "multiplica!" << *cit<<" "<<*it2<<" pos "<<cit.index()<<" "<<it2.index1()<<
          " "<<it2.index2()<< endl;
          //temporal+= *it2 + *cit;
          result(cit.index(),it2.index2())+= *it2 + *cit; ;

        }
      }
      
      /*
            int aij = *it2;
            std::cout << "(" << it2.index1() << "," << it2.index2() << ") = " <<
         *it2
                      << endl;

            */
    }
    // cout << endl;
  }

  /*
    for (auto it1 = c.begin(); it1 != c.end(); ++it1) {
      cout << " (" << *it1 << " : " << it1.index() << ")";
    }
    */
  cout << endl;
}
void mult(const SM &a, const SM &b, SM &result) {
  // result = ublas::prod(a,b);

  // Multiplica matriz a con la matriz b
  cout << "ENTRE AL MULT\n";
  // vector<SM> matrices(a.size2(), SM(b.size1(), 1));
  
  // thread_pool pool;
  
  thread_pool *pool = new thread_pool();

  for (int j = 0; j < b.size2(); j++) {
    concurrentMult(a, j, result, b);
    // cout << j << endl;
    /*auto w = [&a, j, &result, &b]() { concurrentMult(a, j, result, b); };
    pool->submit(w);*/
  }
  cout << "Termina de encolar trabajos\n";

  delete pool;
  
  cout << "AHORA HAGO LA SUMA\n";
  // sparseSum(matrices, result);
}

template <typename T>
void semiring(SparseMatrix<T> &A, SparseMatrix<T> &B, SparseMatrix<T> &C,
              int col) {
  int rows = A.getRows();
  T minimum;
  for (int i = 0; i < rows; i++) {
    minimum = numeric_limits<T>::max();
    for (int j = 0; j < rows; j++)
      if (A.get(i, j) != numeric_limits<T>::max() &&
          B.get(col, j) != numeric_limits<T>::max()) {
        minimum = min(minimum, A.get(i, j) + B.get(col, j));
      }
    C.set(minimum, i, col);
  }
}

/*template <typename T>
void traverseGraph(SparseMatrix<T> &A, SparseMatrix<T> &C) {
  int cols = A.getCols();
  for (int j = 0; j < cols - 1; j++)
    for (int i = 0; i < cols; i++){
      if (j == 0)
        auto w = [&A, &A, &C, j]() {
            semiring(A, A, C, j);
      };
      else
        auto w = [&A, &C, &C, j]() {
            semiring(A, A, C, j);};
      pool->submit(w);
      }
        delete pool;
}*/

void fillMatrix(ublas::compressed_matrix<int> &m, string source) {

  ifstream fin;
  std::string line;

  fin.open(source, ios::in);
  while (std::getline(fin, line)) {
    std::stringstream stream(line);

    char info;
    stream >> info;

    if (info != 'c') {
      size_t i, j, w;
      stream >> i >> j >> w;
      m(i - 1, j - 1) = w;
    }
  }
}

template <typename T> void print(SparseMatrix<T> &mat) {
  for (int i = 0; i < mat.getRows(); i++) {
    for (int j = 0; j < mat.getCols(); j++)
      cout << mat.get(i, j) << " ";
    cout << endl;
  }
}

int main(int argc, char const *argv[]) {
  cout << "AQUI ESTOY\n";
  //ublas::compressed_matrix<int> m(264346, 264346);
  ublas::compressed_matrix<int> m(3, 3);
  //fillMatrix(m, "USA-road-d.NY.gr");
  fillMatrix(m, "test.txt");
  std::cout << "Non-zeroes: " << m.nnz() << '\n'
            << "Allocated storage for " << m.nnz_capacity() << '\n';
  cout << "EMPIEZA LA COSA\n";
  ublas::compressed_matrix<int> c(3, 3);
  //ublas::compressed_matrix<int> c(264346, 264346);

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  mult(m, m, c);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << "duration was : " << duration << endl;
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      cout<<c(i,j)<<" ";
    }
    cout<<endl;
  }

  // fillMatrix<int>(A, "test.txt");
  //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            A.printVal();
  /*
    SparseMatrix<int> c(264346,264346);

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    mult(A,A,c);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t2 - t1).count();
    cout << "duration was : " << duration << endl;
    c.printVal();
  */
  return 0;
}
