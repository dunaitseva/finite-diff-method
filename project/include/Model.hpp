#ifndef FINITEDIFFERENCEMETHOD_MODEL_HPP_
#define FINITEDIFFERENCEMETHOD_MODEL_HPP_

#include <array>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "CalculationUtils.hpp"
#include "Matrix.hpp"
#include "SolutionStorage.hpp"

// As you'll see, I'm a big fan of readable aliases. Don't swear if this makes
// my code unreadable. =)

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
      : m_mesh_ptr_present(),
        m_mesh_ptr_last(),
        m_width(DefModelVal),
        m_height(DefModelVal),
        m_nodes_x(0),
        m_nodes_y(0),
        m_x_delta(0.0),
        m_y_delta(0.0),
        m_time_delta(0.0) {}

  Model(double width, double height, double delta_n, double time_delta);

  /**
   * Set three points of hole geometry
   * @param p1 first point
   * @param p2 second point
   * @param p3 third point
   */
  void SetHoleGeometry(Point p1, Point p2, Point p3);

  /**
   * Sets the initial conditions of the model
   * @param init_conditions Desired initial condition
   */
  void SetInitialCondition(ModelNodeType init_conditions);

  /*
   * Just the few setters for restrictions, that have not specific behavior.
   */
  void SetOuterRestrictions(
      const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_up,
      const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_down,
      const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_left,
      const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_right);
  void SetOuterRestrictions(
      const restr::BoundaryRestrictionsStorageType<ModelNodeType>
          &restrictions);
  void SetInnerRestrictions(
      const restr::BoundaryRestrincionPointerType<ModelNodeType> &restriction);

  void TimeIntegrate(double total_time,
                     solution::SolutionStorageBase<ModelNodeType> &storage);

 private:
  MatrixPointerType m_mesh_ptr_present;
  MatrixPointerType m_mesh_ptr_last;

  double m_width;
  double m_height;
  size_t m_nodes_x;
  size_t m_nodes_y;
  double m_x_delta;
  double m_y_delta;

  double m_time_delta;

  HoleGeometry m_hole_geometry;
  restr::BoundaryRestrictionsStorageType<ModelNodeType> m_outer_restrictions;
  restr::BoundaryRestrincionPointerType<ModelNodeType> m_inner_restriction;

  // Bellow functions helps to determine hole and boundary points related to
  // hole
  [[nodiscard]] bool PointInHole(Point point) const;
  [[nodiscard]] bool PointOnBorder(Point point) const;
  // Calculates auxiliary values for PointInHole and PointOnBorder function
  [[nodiscard]] std::tuple<ModelNodeType, ModelNodeType, ModelNodeType>
  CalcCheckValues(
      Point point) const;  // sorry for that, it's just a formatter :)))
  ModelNodeType GetInnerNeighbor(size_t x_shift, size_t y_shift);
};

namespace exceptions {
class ModelBaseException : std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Model exception occur";
  }
};

class WrongDeltaRel : std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Error: (dt / dx) ^ 2 > 1 / 2";
  }
};
}  // namespace exceptions
}  // namespace fdm

#endif  // FINITEDIFFERENCEMETHOD_MODEL_HPP_
