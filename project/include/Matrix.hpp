#ifndef FINITEDIFFERENCEMETHOD_MATRIX_HPP_
#define FINITEDIFFERENCEMETHOD_MATRIX_HPP_

#include <array>
#include <cstddef>
#include <exception>
#include <memory>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace mtrx {
/**
 * I do not really want to complicate the code in the context of my tasks.
 * Therefore, I add the implementation of the store and accessor as much as
 * possible. But MatrixBase provide the simple interface, that using in
 * FDM implementation.
 *
 * @note All derived classes have to necessary define Type
 *
 * @tparam Tp matrix elements type
 */
namespace base {
template <typename Tp>
class MatrixBase {
 public:
  using Type = Tp;
  virtual void SetValue(size_t row, size_t col, Tp &&val) = 0;
  virtual const Tp &GetValue(size_t row, size_t col) const = 0;
  virtual size_t SizeRows() const = 0;
  virtual size_t SizeCols() const = 0;
  [[maybe_unused]] virtual void FillMatrix([[maybe_unused]] Tp val) {}

  virtual ~MatrixBase() = default;
};

template <typename Tp>
class MatrixDynamicBase : public MatrixBase<Tp> {
 public:
  using Type = Tp;
  virtual void SetRows(size_t rows) = 0;
  virtual void SetCols(size_t cols) = 0;
  void SetSize(size_t rows, size_t cols) {
    this->SetRows(rows);
    this->SetCols(cols);
  }

  virtual ~MatrixDynamicBase() = default;
};
}  // namespace base

/**
 * Simplest matrix implementation. Matrix class in this program maintains
 * only basic operations and using as structure for easy to store and
 * access values.
 *
 * @tparam Tp matrix elements type
 * @tparam Cols amount of columns
 * @tparam Rows amount of m_rows
 */
template <typename Tp, size_t Rows, size_t Cols>
class Matrix : public base::MatrixBase<Tp> {
 public:
  using Type = Tp;
  using MatrixStorageType = std::array<Type, Cols * Rows>;
  static constexpr auto MatrixAccessor = [](size_t row, size_t col) -> size_t {
    return row * Cols + col;
  };

  void SetValue(size_t row, size_t col, Tp &&val) override;
  const Tp &GetValue(size_t row, size_t col) const override;
  [[nodiscard]] virtual size_t SizeRows() const override { return Rows; }
  [[nodiscard]] virtual size_t SizeCols() const override { return Cols; }

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

/**
 * This matrix builder is created only because I started implemented
 * current library with only static matrix
 */
template <typename MatrixClass>
class MatrixCreatorStatic {
 public:
  using BaseType = base::MatrixBase<typename MatrixClass::Type>;
  using MatrixPointer = std::unique_ptr<BaseType>;

  MatrixPointer operator()() {
    static_assert(std::is_base_of_v<BaseType, MatrixClass>);
    return std::make_unique<MatrixClass>();
  }
};

template <typename Tp>
class MatrixDynamic : public base::MatrixDynamicBase<Tp> {
 public:
  using Type = Tp;
  using MatrixStorageType = std::vector<Type>;

  void SetRows(size_t rows) override {
    m_rows = rows;
    m_storage.resize(m_rows * m_cols);
  }
  void SetCols(size_t cols) override {
    m_cols = cols;
    m_storage.resize(m_rows * m_cols);
  }
  void SetSize(size_t rows, size_t cols) {
    m_rows = rows;
    m_cols = cols;
    m_storage.resize(rows * cols);
  }
  [[nodiscard]] virtual size_t SizeRows() const override { return m_rows; }
  [[nodiscard]] virtual size_t SizeCols() const override { return m_cols; }

  void SetValue(size_t row, size_t col, Tp &&val) override;
  const Tp &GetValue(size_t row, size_t col) const override;
  void FillMatrix(Tp val) override;

  MatrixDynamic() = default;
  MatrixDynamic(size_t rows, size_t cols) { SetSize(rows, cols); }

 private:
  MatrixStorageType m_storage;
  size_t m_cols;
  size_t m_rows;

  [[nodiscard]] bool CheckAccess(size_t row, size_t col) const {
    return col < m_cols && row < m_rows;
  }

  [[nodiscard]] size_t MatrixAccessor(size_t row, size_t col) const {
    return row * m_cols + col;
  }
};

template <typename Tp>
void MatrixDynamic<Tp>::SetValue(size_t row, size_t col, Tp &&val) {
  if (!CheckAccess(row, col)) {
  }
  m_storage[MatrixAccessor(row, col)] = std::forward<Tp>(val);
}

template <typename Tp>
const Tp &MatrixDynamic<Tp>::GetValue(size_t row, size_t col) const {
  if (!CheckAccess(row, col)) {
  }
  return m_storage[MatrixAccessor(row, col)];
}

template <typename Tp>
void MatrixDynamic<Tp>::FillMatrix(Tp val) {
  std::fill(m_storage.begin(), m_storage.end(), val);
}

class MatrixCreatorDynamic {
 public:
  template <typename Tp>
  using TargetType = MatrixDynamic<Tp>;
  template <typename Tp>
  using BaseType = base::MatrixDynamicBase<Tp>;
  template <typename Tp>
  using Pointer = std::unique_ptr<BaseType<Tp>>;

  template <typename Tp>
  Pointer<Tp> Build() {
    return std::forward<Pointer<Tp>>(std::make_unique<TargetType<Tp>>());
  }
};

namespace exceptions {
class BaseMatrixException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Matrix error occur";
  }
};

class MatrixSizeException : public BaseMatrixException {
  [[nodiscard]] const char *what() const noexcept override {
    return "Matrix size error occur. You may have gone beyond the matrix.";
  }
};
}  // namespace exceptions
}  // namespace mtrx

#endif  // FINITEDIFFERENCEMETHOD_MATRIX_HPP_
