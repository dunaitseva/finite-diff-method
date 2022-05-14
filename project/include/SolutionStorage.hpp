#ifndef FINITEDIFFERENCEMETHOD_SOLUTIONSTORAGE_HPP_
#define FINITEDIFFERENCEMETHOD_SOLUTIONSTORAGE_HPP_

#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string_view>

#include "Matrix.hpp"

namespace fdm {
namespace solution {
namespace exceptions {
class SolutionStorageException : public std::exception {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return "Storage exception occur";
  }
};

class FileNotOpenException : public std::exception {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return "One of files (configuration or plot) was not open";
  }
};

}  // namespace exceptions
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

template <typename MeshNodesType>
class StaticGnuplotHeatmapStorage : public SolutionStorageBase<MeshNodesType> {
 public:
  using StringType = std::string;

  explicit StaticGnuplotHeatmapStorage(
      const StringType &file_prefix)
      : m_file_output_prefix(file_prefix) {
    m_plot_file_name =
        std::string(m_file_output_prefix) + std::string(plot_extension);
    m_config_file_name =
        std::string(m_file_output_prefix) + std::string(config_extension);

    m_plot_output.open(m_plot_file_name.data());
    m_config_output.open(m_config_file_name.data());
    if (!m_plot_output.is_open() || !m_config_output.is_open()) {
      throw exceptions::FileNotOpenException();
    }
    WriteConfig();
  }

  void CommitLayer(
      const typename SolutionStorageBase<MeshNodesType>::MeshPointerType
          &mesh_ptr) override {
    std::ostream &StandardStreamDefinition = m_plot_output;
    for (size_t i = 0; i < mesh_ptr->SizeRows(); ++i) {
      for (size_t j = 0; j < mesh_ptr->SizeCols(); ++j) {
        StandardStreamDefinition << std::setw(8) << mesh_ptr->GetValue(i, j) << ' ';
      }
      StandardStreamDefinition << '\n';
    }
    StandardStreamDefinition << '\n';
  }

  ~StaticGnuplotHeatmapStorage() override {
    m_plot_output.close();
    m_config_output.close();
  }

 private:
  StringType m_file_output_prefix;
  StringType m_plot_file_name;
  StringType m_config_file_name;
  std::ofstream m_config_output;
  std::ofstream m_plot_output;

  StringType plot_extension = ".plt";
  StringType config_extension = ".cfg";
  StringType mapping_extension = ".png";
  StringType default_file_prefix = "def_output";

  void WriteConfig() {
    StringType terminal_def{"set terminal png\n"};
	StringType output_file_def{
        "set output '" + std::string(m_file_output_prefix) +
        std::string(mapping_extension) + "'\n"};
	StringType scale_def{
        "set autoscale yfix\nset autoscale xfix\n"};
	StringType palette_def{
        "set palette defined (0 0 0 0.5, 1 0 0 1, 2 0 0.5 1, 3 0 1 1, 4 0.5 1 "
        "0.5, 5 1 1 0, 6 1 0.5 0, 7 1 0 0, 8 0.5 0 0)\n"};
	StringType map_type_def{"set pm3d map\n"};
	StringType splot_def{
        "splot '" + std::string(m_plot_file_name) + "' matrix notitle\n"};
    m_config_output << terminal_def;
    m_config_output << output_file_def.data();
    m_config_output << scale_def;
    m_config_output << palette_def;
    m_config_output << map_type_def;
    m_config_output << splot_def.data();
  }
};

template <typename MeshNodesType>
class PlaceholderStorage : public SolutionStorageBase<MeshNodesType> {
 public:
  void CommitLayer(
      const typename SolutionStorageBase<MeshNodesType>::MeshPointerType
          &mesh_ptr) override {}
};
}  // namespace solution
}  // namespace fdm

#endif  // FINITEDIFFERENCEMETHOD_SOLUTIONSTORAGE_HPP_
