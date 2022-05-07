#ifndef FINITEDIFFERENCEMETHOD_SOLUTIONSTORAGE_HPP_
#define FINITEDIFFERENCEMETHOD_SOLUTIONSTORAGE_HPP_

#include <iomanip>
#include <iostream>
#include <memory>

#include "Matrix.hpp"

namespace fdm {
namespace solution {
/**
 * Basic model's mesh storing class. Class provides functions, that helps
 * to manage mesh optimal storage. This class is necessary to use if you
 * dont want to lose your calculations data, because Model class don't
 * store all time integrated layers.
 *
 * @tparam MeshNodesType specify mesh nodes type
 */
template <typename MeshNodesType>
class SolutionStorageBase {
 public:
  using MeshType = mtrx::base::MatrixBase<MeshNodesType>;
  using MeshPointerType = std::shared_ptr<MeshType>;

  virtual void CommitLayer(const MeshPointerType &mesh_ptr) = 0;
  virtual ~SolutionStorageBase() = default;
};

/**
 * Simplest storage, that commit all integrated layers of
 * model in standard output
 */
template <typename MeshNodesType>
class StandardStreamStorage : public SolutionStorageBase<MeshNodesType> {
 public:
  std::ostream &StandardStreamDefinition = std::cout;

  void CommitLayer(
      const typename SolutionStorageBase<MeshNodesType>::MeshPointerType
          &mesh_ptr) override {
//    StandardStreamDefinition.precision(2);
    for (size_t i = 0; i < mesh_ptr->SizeRows(); ++i) {
      for (size_t j = 0; j < mesh_ptr->SizeCols(); ++j) {
        StandardStreamDefinition << mesh_ptr->GetValue(i, j) << ' ';
      }
      StandardStreamDefinition << '\n';
    }
    StandardStreamDefinition << '\n';
  }
};
}  // namespace solution
}  // namespace fdm

#endif  // FINITEDIFFERENCEMETHOD_SOLUTIONSTORAGE_HPP_
