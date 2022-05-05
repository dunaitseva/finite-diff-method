#include <iostream>
#include <iomanip>

#include "Matrix.hpp"

int main() {
  mtrx::MatrixCreatorDynamic matrix_builder;
  auto matrix = matrix_builder.Build<double>();
  matrix->SetSize(13, 12);
  matrix->SetValue(0, 0, 1.2);
  matrix->SetValue(1, 2, 0.2);
  matrix->SetValue(5, 7, 0.7);

  for (size_t i = 0; i < matrix->SizeRows(); ++i) {
	for (size_t j = 0; j < matrix->SizeCols(); ++j) {
	  std::cout << std::setw(4) << matrix->GetValue(i, j) << ' ';
	}
	std::cout << '\n';
  }
}