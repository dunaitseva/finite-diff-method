#include <iostream>

#include "Matrix.hpp"

int main() {
  mtrx::Matrix<double, 13, 23> matrix;
  matrix.SetValue(0, 0, 0.0);
  matrix.SetValue(0, 1, 0.2);
  matrix.SetValue(0, 2, 5.2);

  std::cout << matrix.GetValue(0, 0) << std::endl;
  std::cout << matrix.GetValue(0, 1) << std::endl;
  std::cout << matrix.GetValue(0, 2) << std::endl;
}