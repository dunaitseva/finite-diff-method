#ifndef FINITEDIFFERENCEMETHOD_MATRIX_HPP_
#define FINITEDIFFERENCEMETHOD_MATRIX_HPP_

#include <array>
#include <cstddef>
#include <exception>
#include <memory>

namespace mtrx {
/**
 * I do not really want to complicate the code in the context of my tasks.
 * Therefore, I add the implementation of the store and accessor as much as
 * possible. But MatrixBase provide the simple interface, that using in
 * FDM implementation.
 *
 * @tparam Tp matrix elements type
 */
template <typename Tp>
class MatrixBase {
 public:
  virtual void SetValue(size_t row, size_t col, Tp &&val) = 0;
  virtual const Tp &GetValue(size_t row, size_t col) const = 0;
};

/**
 * Simplest matrix implementation. Matrix class in this program maintains
 * only basic operations and using as structure for easy to store and
 * access values.
 *
 * @tparam Tp matrix elements type
 * @tparam Cols amount of columns
 * @tparam Rows amount of rows
 */
template <typename Tp, size_t Rows, size_t Cols>
class Matrix : public MatrixBase<Tp> {
 public:
  using MatrixStorageType = std::array<Tp, Cols * Rows>;
  static constexpr auto MatrixAccessor = [](size_t row, size_t col) -> size_t {
    return row * Cols + col;
  };

  void SetValue(size_t row, size_t col, Tp &&val) override;
  const Tp &GetValue(size_t row, size_t col) const override;

  Matrix() = default;

 private:
  MatrixStorageType m_storage;

  [[nodiscard]] bool CheckAccess(size_t row, size_t col) const {
    return col < Cols && row < Rows;
  }
};

template <typename Tp, size_t Rows, size_t Cols>
void Matrix<Tp, Rows, Cols>::SetValue(size_t row, size_t col, Tp &&val) {
  if (!CheckAccess(row, col)) {
  }
  m_storage[MatrixAccessor(row, col)] = std::forward<Tp>(val);
}

template <typename Tp, size_t Rows, size_t Cols>
const Tp &Matrix<Tp, Rows, Cols>::GetValue(size_t row, size_t col) const {
  if (!CheckAccess(row, col)) {
  }
  return m_storage[MatrixAccessor(row, col)];
}

namespace exceptions {
class BaseMatrixException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Matrix error occur";
  }
};

class MatrixSizeException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Matrix size error occur. You may have gone beyond the matrix.";
  }
};

}  // namespace exceptions
}  // namespace mtrx

#endif  // FINITEDIFFERENCEMETHOD_MATRIX_HPP_
