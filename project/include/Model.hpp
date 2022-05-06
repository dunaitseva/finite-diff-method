#ifndef FINITEDIFFERENCEMETHOD_MODEL_HPP_
#define FINITEDIFFERENCEMETHOD_MODEL_HPP_

#include <array>
#include <utility>
#include <vector>
#include <fstream>

#include "Matrix.hpp"

namespace fdm {
/**
 * Since finite difference coding in general cannot be done for
 * free-form geometry, I wrote a model that solves the
 * non-stationary heat conduction problem in an explicit way on
 * the geometry of a tube with a triangular hole.
 */
class Model {
 public:
  /*
   * If you wish to change the implementation of the class with
   * your own data types that implement the interface declared
   * in the Matrix.h file, change the aliases declared here.
   * This is done so as not to complicate the understanding of
   * the program code.
   */
  using ModelNodeType = double;
  constexpr static ModelNodeType DefModelVal = 0.0;
  using MatrixBuilder = mtrx::MatrixCreatorDynamic;
  using MatrixPointerType = MatrixBuilder::Pointer<ModelNodeType>;

  /*
   * Actually I could use std::pair, but in my opinion Point
   * is more representative.
   */
  struct Point {
    ModelNodeType x;
    ModelNodeType y;
    Point() : x(DefModelVal), y(DefModelVal) {}
    Point(ModelNodeType _x, ModelNodeType _y) : x(_x), y(_y) {}
  };
  /*
   * Triangular hole in the center of tube. I think
   * good idea to store it in fixed size array.
   */
  using HoleGeometry = std::array<Point, 3>;

  // Finally, after all this NECESSARY definitions - code!!!
  Model()
      : m_mesh_ptr(),
        m_width(DefModelVal),
        m_height(DefModelVal),
        m_nodes_x(0),
        m_nodes_y(0),
        m_x_delta(0.0),
        m_y_delta(0.0),
        m_total_time(0.0),
        m_time_delta(0.0),
		m_time_iterations(0) {}

  Model(double width, double height, size_t nodes_x, size_t nodes_y,
        double total_time, double time_delta);

  void SetHoleGeometry(Point p1, Point p2, Point p3);
  void SetInitialCondition(ModelNodeType init_conditions);

 private:
  MatrixPointerType m_mesh_ptr;

  double m_width;
  double m_height;
  size_t m_nodes_x;
  size_t m_nodes_y;
  double m_x_delta;
  double m_y_delta;

  double m_total_time;
  double m_time_delta;
  size_t m_time_iterations;

  HoleGeometry m_hole_geometry;
};
}  // namespace fdm

#endif  // FINITEDIFFERENCEMETHOD_MODEL_HPP_
