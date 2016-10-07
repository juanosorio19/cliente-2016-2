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
    unsigned const thread_count =std::thread::hardware_concurrency();
    cout << "threads in pool " << thread_count << endl;
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

void concurrentMult(const SM &a, const int col, SM &result, const SM &b,mutex &mtx) {

  auto c = row(b, col);

  for (auto cit =  c.begin(); cit != c.end(); ++cit) {
    int temporal =0;
    for (auto it1 = a.begin1(); it1 != a.end1(); ++it1) {
      for (auto it2 = it1.begin(); it2 != it1.end(); ++it2) {
        if (it2.index2() == cit.index()) {
          temporal+= *it2 * c[it2.index1()];
          /*cout << "multiplica!" <<c[it2.index1()] <<" "<<*it2<<" pos "<<cit.index()<<" "<<it2.index1()<<
          "                         "<<col<<" "<< it2.index2()<<"   "<<result(col,it2.index2())<< endl;*/

        }
      }
    }

    mtx.lock();
    result(col,cit.index())= temporal;
    mtx.unlock();
  }
}

void mult(const SM &a, const SM &b, SM &result) {
  cout << "ENTRE AL MULT\n";
  thread_pool *pool = new thread_pool();
  mutex mtx;

  for (int j = 0; j < b.size2(); j++) {
    //concurrentMult(a, j, result, b,mtx);
    auto w = [&a, j, &result, &b,&mtx]() {
      concurrentMult(a, j, result, b,mtx);
      // c = concurrentMult(a, j, result, b,mtx);
      // mutex
      // result(fila) = c
      // end mutex
    };
    pool->submit(w);
  }
  //cout << "Termina de encolar trabajos\n";

  delete pool;

  cout << "AHORA HAGO LA SUMA\n";
  // sparseSum(matrices, result);
}

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

int main(int argc, char const *argv[]) {
  cout << "AQUI ESTOY\n";
  ublas::compressed_matrix<int> m(264346, 264346);
  //ublas::compressed_matrix<int> m(3, 3);
  fillMatrix(m, "USA-road-d.NY.gr");
  //fillMatrix(m, "test.txt");
  std::cout << "Non-zeroes: " << m.nnz() << '\n'
            << "Allocated storage for " << m.nnz_capacity() << '\n';
  cout << "EMPIEZA LA COSA\n";
  //ublas::compressed_matrix<int> c(3, 3);
  ublas::compressed_matrix<int> c(264346, 264346);

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  mult(m, m, c);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << "duration was : " << duration << endl;

/*
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      cout<<c(i,j)<<" ";
    }
    cout<<endl;
  }
*/
  return 0;
}
