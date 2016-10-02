#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

template <typename T>
class SparseMatrix {
 private:
  int rows;
  int cols;
  vector<T> vals;
  vector<int> colInd;
  vector<int> rowPtr;

 public:
  SparseMatrix() {}
  SparseMatrix(int r, int c) : rows(r), cols(c), rowPtr(r + 1, -1) {}

  int getRows() { return rows; }

  int getCols() { return cols; }

  T get(int r, int c) {
    int firstEl = rowPtr[r];
    int col = colInd[firstEl];
    int diffCol = 0;  // difference between the first element in the row and c
    int diffVal = 0;  // differences between the values in colInd
    int index = 0;
    if (firstEl == -1) return 0;
    if (col > c) return 0;
    if (col == c)
      return vals[firstEl];
    else {
      diffCol = c - col;
      diffVal = colInd[firstEl + diffCol] - col;
      if (diffCol != diffVal)
        return 0;
      else
        return vals[firstEl + diffCol];
    }
  }

  void set(T val, int r, int c) {
    vals.push_back(val);
    colInd.push_back(c);
    if (rowPtr[r] == -1) rowPtr[r] = vals.size() - 1;
    if (r == rows - 1 && c == cols - 1) rowPtr[rowPtr.size() - 1] = vals.size();
  }

  void rowMultMatrix(SparseMatrix<T> &b, SparseMatrix<T> &c, int indRow) {
    int temp;
    for (int i = 0; i < b.cols; i++) {
      temp = 0;
      if (rowPtr[i] != -1)
        for (int j = 0; j < cols; j++)
          temp = temp + (get(indRow, j) * b.get(j, i));
      c.set(temp, indRow, i);
    }
  }

  SparseMatrix<T> mult(SparseMatrix<T> &b) {
    /*int conc = thread::hardware_concurrency();
    SparseMatrix<T> c(rows, b.cols);
    vector<thread> threads;
    for (int i = 0; i < rows; i++) {
      threads.push_back(thread(rowMultMatrix, this, b, c, i));
    }
    for (int i = 0; i < conc; i++) {
      threads[i].join();
    }*/

    int temp;
    SparseMatrix<T> c(rows, b.cols);
    for (int i = 0; i < rows; i++)
      if (rowPtr[i] != -1)
        for (int j = 0; j < b.cols; j++) {
          temp = 0;
          for (int k = 0; k < b.rows; k++)
            temp = temp + (get(i, k) * b.get(k, j));
          c.set(temp, i, j);
        }
    return c;
  }
};

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
      mat.set(iterator, i, j);
    }
}

int main() {
  // WARNING: the template args of SparseMatrix and fillMatrix must be the
  // same
  SparseMatrix<int> A;
  SparseMatrix<int> B;
  fillMatrix<int>(A, "graph.txt");
  fillMatrix<int>(B, "graph.txt");
  SparseMatrix<int> C(A.getRows(), B.getCols());
  C = A.mult(B);
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      cout << C.get(i, j) << " ";
    }
    cout << endl;
  }
  return 0;
}
