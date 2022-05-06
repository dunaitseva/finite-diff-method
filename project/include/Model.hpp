#ifndef FINITEDIFFERENCEMETHOD_MODEL_HPP_
#define FINITEDIFFERENCEMETHOD_MODEL_HPP_

#include <array>
#include <fstream>
#include <utility>
#include <vector>
#include <iostream>
#include <tuple>
#include <exception>

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

  void TimeIntegrate(double total_time);

  void TemplePrint() {
	for (size_t i = 0; i < m_mesh_ptr_last->SizeRows(); ++i) {
	  for (size_t j = 0; j < m_mesh_ptr_last->SizeCols(); ++j) {
		double _x = static_cast<double>(j) * m_x_delta;
		double _y = static_cast<double>(i) * m_y_delta;
		Point point(_x, _y);
		if (PointOnBorder(point)) {
		  std::cout << "GG ";
		} else if (PointInHole(point)) {
		  std::cout << "XX ";
		} else {
		  std::cout << m_mesh_ptr_last->GetValue(i, j) << ' ';
		}
	  }
	  std::cout << '\n';
	}
  }

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

  // Bellow functions helps to determine hole and boundary points related to hole
  bool PointInHole(Point point) const;
  bool PointOnBorder(Point point) const;
  // Calculates auxiliary values for PointInHole and PointOnBorder function
  std::tuple<ModelNodeType, ModelNodeType, ModelNodeType> CalcCheckValues(Point point) const;
};

namespace exceptions {
class ModelBaseException : std::exception {
  [[nodiscard]] const char * what() const noexcept override {
	return "Model exception occur";
  }
};

class WrongDeltaRel : std::exception {
  [[nodiscard]] const char * what() const noexcept override {
	return "Error: (dt / dx) ^ 2 > 1 / 2";
  }
};
}
}  // namespace fdm

#endif  // FINITEDIFFERENCEMETHOD_MODEL_HPP_
