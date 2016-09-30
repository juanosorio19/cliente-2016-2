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

class join_threads {
  std::vector<std::thread> &threads;

 public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
    // std::cerr << "destructing joiner\n";
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable()) threads[i].join();
    }
  }
};

template <typename T>
class threadsafe_queue {
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
    if (data_queue.empty()) return false;
    value = std::move(data_queue.front());
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty()) return std::shared_ptr<T>();
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
      done = true;
      throw;
    }
  }
  ~thread_pool() {
    joiner->~join_threads();
    done = true;
    // std::string s("Destructing pool ");
    // s += std::to_string(work_queue.empty());
    // s += '\n';
    // std::cerr << s;
  }
  template <typename FunctionType>
  void submit(FunctionType f) {
    work_queue.push(std::function<void()>(f));
    //    std::cerr << std::this_thread::get_id() << std::endl;
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
  SparseMatrix() {}
  SparseMatrix(int r, int c) : rows(r), cols(c), rowPtr(r + 1, 0) {}
  int getRows() { return rows; }
  int getCols() { return cols; }
  vector<T> getVal() { return val; }

  T get(int r, int c) {
    // Retorna el elemento que hay en la posici ́on (r,c)
    // asercion= r,c menores o iguales que rows,cols
    int rowNumber =
        rowPtr[r + 1] - rowPtr[r];  // se cuantos elementos hay en la fila r
    int start = rowPtr[r];  // se cuantos elementos hay al empezar la fila r
    for (int i = 0; i < rowNumber; i++) {
      if (colInd[i + start] == c)
        return val[i + start];  // tengo el elemento en la posicion r,c
    }
    return 0;  // element(r,c)=0
  }
  // retorna un vector columna (c)
  vector<T> getCol(int c) {
    vector<T> result(this->rows, 0);
    for (int i = 0; i <= this->rows; i++) {
      if (get(i, c) != 0) {
        result[i] = get(i, c);
      }
    }
    return result;
  }

  void set(T value, int r, int c) {
    int rn = rowPtr[r + 1] - rowPtr[r];  // cuantos elementos hay en fila r
    int start = rowPtr[r];
    if (get(r, c) != 0) {
      for (int i = 0; i < rn; i++) {
        if (colInd[i + start] == c)
          val[i + start] = value;  // tengo el elemento en la posicion r,c
      }
      return;
    } else if (rn == 0) {  // es el primer valor de la fila r
      val.emplace(val.begin() + start, value);
      colInd.emplace(colInd.begin() + start, c);

    } else {
      for (int i = 0; i < rn; i++) {
        if (c < colInd[rowPtr[r] + i]) {
          val.emplace(val.begin() + start + i, value);
          colInd.emplace(colInd.begin() + start + i, c);
          for (int j = r + 1; j < rowPtr.size();
               j++) {  // redimensiono rowPtr desde r+1
            rowPtr[j] += 1;
          }
          return;
        }
      }
      val.emplace(val.begin() + start + rn, value);
      colInd.emplace(colInd.begin() + start + rn, c);
    }
    for (int j = r + 1; j < rowPtr.size();
         j++) {  // redimensiono rowPtr desde r+1
      rowPtr[j] += 1;
    }
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

  SparseMatrix semiring(SparseMatrix &A) {
    T minimum = 0;
    int r = getRows();
    int c = getCols();

    SparseMatrix result(r, c);

    for (int i = 0; i < r; ++i)
      for (int j = 0; j < c; ++j) {
        minimum = numeric_limits<T>::max();
        for (int k = 0; k < r; ++k) {
          if (get(i, k) != numeric_limits<T>::max() &&
              A.get(k, j) != numeric_limits<T>::max()) {
            minimum = min(minimum, (get(i, k) + A.get(k, j)));
            cout << "A[" << i << "][" << k << "] = " << get(i, k) << endl;
            cout << "DEBUG: " << get(0, 1) << endl;
            cout << "B[" << k << "][" << j << "] = " << A.get(k, j) << endl;
            cout << "current: " << get(i, k) + A.get(k, j) << endl;
          }
        }
        result.set(i, j, minimum);
        cout << "pos " << i << " " << j << ": " << minimum << endl;
        // result.setData(j, i, minimum);
      }
    result.set(0, 0, 1);
    result.set(0, 1, 2);
    result.set(0, 2, 3);
    print(result);
    return result;
  }
};

void concurrentMult(SparseMatrix<int> &a, vector<int> &b,
                    SparseMatrix<int> &result, int col) {
  // Multiplica matriz a *fila

  for (int i = 0; i < a.getRows(); i++) {
    for (int j = 0; j < a.getCols(); j++) {
      for (int k = 0; k < b.size(); k++) {
        if ((a.get(i, k) and b[k]) != 0)
          result.set(result.get(i, col) + (a.get(i, k) * b[k]), i, col);
      }
    }
  }
}
void mult(SparseMatrix<int> &a, SparseMatrix<int> &b,
          SparseMatrix<int> &result) {
  // Multiplica matriz a con la matriz b
  {
    thread_pool pool;
    for (int j = 0; j < b.getRows(); j++) {
      vector<int> temporal = a.getCol(j);
      auto w = [&a, &temporal, &result, j]() {
        concurrentMult(a, temporal, result, j);
      };
      pool.submit(w);
    }
  }
}

template <typename T>
void fillMatrix(SparseMatrix<T> &mat, string source) {
  int rows = 0;
  int cols = 0;
  T iterator;
  ifstream fin;
  fin.open(source.c_str(), ios::in);
  fin >> iterator;
  rows = iterator;
  fin >> iterator;
  cols = iterator;
  mat = SparseMatrix<T>(rows, cols);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      fin >> iterator;
      if (iterator != -1)
        mat.set(iterator, i, j);
      else
        mat.set(numeric_limits<T>::max(), i, j);
    }
}

template <typename T>
void print(SparseMatrix<T> &mat) {
  for (int i = 0; i < mat.getRows(); i++) {
    for (int j = 0; j < mat.getCols(); j++) {
      cout << mat.get(i, j) << " ";
    }
    cout << endl;
  }
}

template <typename T>
void semiring(SparseMatrix<T> &A, SparseMatrix<T> &C) {
  for (int i = 0; i < A.getCols(); A++) path(A, col, C);
}

int main(int argc, char const *argv[]) {
  SparseMatrix<int> A;
  fillMatrix<int>(A, "graph.txt");
  SparseMatrix<int> C(A.getRows(), A.getCols());
  cout << "DEBUG: " << A.get(0, 1) << endl;
  semiring(A, C);
  // print(A);
  // print(B);
  // print(C);
  // SparseMatrix<int> a(3,3);

  /*a.set(1, 0, 0);
  a.set(9, 2, 2);
  a.set(6, 1, 2);
  a.set(4, 1, 0);
  a.set(2, 0, 1);
  a.set(8, 2, 1);
  a.set(3, 0, 2);
  a.set(5, 1, 1);
  a.set(7, 2, 0);

  SparseMatrix<int> ar(3, 3);

  ar.set(1, 0, 0);
  ar.set(2, 0, 1);
  ar.set(3, 0, 2);
  ar.set(4, 1, 0);
  ar.set(5, 1, 1);
  ar.set(6, 1, 2);
  ar.set(7, 2, 0);
  ar.set(8, 2, 1);
  ar.set(9, 2, 2);
  vector<int> aval = a.getVal();

  for (int i = 0; i <= a.getCols() * a.getCols() - 1; i++) {
    cout << aval[i] << " ";
  }
  cout << endl;
  // cout<<aval.size();

  SparseMatrix<int> c(3, 3);
  SparseMatrix<int> result(3, 3);

  /*mult(a,a,c);
  vector<int> cval=c.getVal();

  for(int i=0;i<=c.getCols()*c.getCols()-1;i++){
    cout<<cval[i]<<" ";

  }
  a.mult(ar,result);
  vector<int> rval=result.getVal();
  for(int i=0;i<=result.getCols()*result.getCols()-1;i++){
    cout<<rval[i]<<" ";
  }
 // cout<<endl<<a.get(2,2);*/

  return 0;
}
