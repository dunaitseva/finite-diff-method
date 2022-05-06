#include "Model.hpp"

#include <iostream>
#include <vector>

namespace fdm {
namespace {
bool IsInHole(const std::vector<Model::ModelNodeType> &values) {
  size_t sign_counter = 0;
  for (const Model::ModelNodeType &item : values) {
    if (item < 0) {
      ++sign_counter;
    }
  }

  return (sign_counter == 3 || sign_counter == 0);
}
}  // anonymous namespace

Model::Model(double width, double height, double delta_n, double time_delta)
    : m_mesh_ptr_present(MatrixBuilder().Build<ModelNodeType>()),
      m_mesh_ptr_last(MatrixBuilder().Build<ModelNodeType>()),
      m_width(width),
      m_height(height),
      m_x_delta(delta_n),
      m_y_delta(delta_n),
      m_time_delta(time_delta) {
  if ((m_time_delta / m_x_delta) * (m_time_delta / m_x_delta) > 0.5) {
    throw exceptions::WrongDeltaRel();
  }
  m_nodes_x = static_cast<size_t>(m_width / m_x_delta);
  m_nodes_y = static_cast<size_t>(m_height / m_y_delta);
  m_mesh_ptr_present->SetSize(m_nodes_y, m_nodes_x);
  m_mesh_ptr_last->SetSize(m_nodes_y, m_nodes_x);

  m_hole_geometry[0] = Point();
  m_hole_geometry[1] = Point();
  m_hole_geometry[2] = Point();
}

void Model::SetHoleGeometry(Point p1, Point p2, Point p3) {
  m_hole_geometry[0] = p1;
  m_hole_geometry[1] = p2;
  m_hole_geometry[2] = p3;
}

void Model::SetInitialCondition(ModelNodeType init_conditions) {
  m_mesh_ptr_present->FillMatrix(init_conditions);
  m_mesh_ptr_last->FillMatrix(init_conditions);
}

void Model::TimeIntegrate(double total_time) {
  auto time_integrate_iterations =
      static_cast<size_t>(total_time / m_time_delta);
  for (size_t i = 0; i < time_integrate_iterations; ++i) {

  }
}

std::tuple<Model::ModelNodeType, Model::ModelNodeType, Model::ModelNodeType>
Model::CalcCheckValues(Point point) const {
  ModelNodeType check_val1 = (m_hole_geometry[0].x - point.x) *
                                 (m_hole_geometry[1].y - m_hole_geometry[0].y) -
                             (m_hole_geometry[1].x - m_hole_geometry[0].x) *
                                 (m_hole_geometry[0].y - point.y);
  ModelNodeType check_val2 = (m_hole_geometry[1].x - point.x) *
                                 (m_hole_geometry[2].y - m_hole_geometry[1].y) -
                             (m_hole_geometry[2].x - m_hole_geometry[1].x) *
                                 (m_hole_geometry[1].y - point.y);
  ModelNodeType check_val3 = (m_hole_geometry[2].x - point.x) *
                                 (m_hole_geometry[0].y - m_hole_geometry[2].y) -
                             (m_hole_geometry[0].x - m_hole_geometry[2].x) *
                                 (m_hole_geometry[2].y - point.y);
  return {check_val1, check_val2, check_val3};
}

bool Model::PointInHole(Point point) const {
  /*
   * Mathematical part - vector and pseudoscalar product.
   * Implementation - products are considered (1,2,3 - triangle vertices, 0 -
   * point): (x1-x0)*(y2-y1)-(x2-x1)*(y1-y0) (x2-x0)*(y3-y2)-(x3-x2)*(y2-y0)
   * (x3-x0)*(y1-y3)-(x1-x3)*(y3-y0)
   * If they are of the same sign, then the point is inside
   * the triangle, otherwise the point is outside the triangle.
   */
  auto [check_val1, check_val2, check_val3] = CalcCheckValues(point);

  // The comparison described above takes place in the function IsInHole
  return IsInHole({check_val1, check_val2, check_val3});
}

bool Model::PointOnBorder(Point point) const {
  /*
   * If one of bellow values is zero, then the point
   * lies on the side
   */

  //   Check neighbor points
  if (!PointInHole(point)) {
    if (PointInHole(Point(point.x, point.y - m_y_delta))) {
      return true;
    }
    if (PointInHole(Point(point.x, point.y + m_y_delta))) {
      return true;
    }
    if (PointInHole(Point(point.x - m_x_delta, point.y))) {
      return true;
    }
    if (PointInHole(Point(point.x + m_x_delta, point.y))) {
      return true;
    }

    if (PointInHole(Point(point.x + m_x_delta, point.y - m_y_delta))) {
      return true;
    }
    if (PointInHole(Point(point.x + m_x_delta, point.y + m_y_delta))) {
      return true;
    }
    if (PointInHole(Point(point.x - m_x_delta, point.y - m_y_delta))) {
      return true;
    }
    if (PointInHole(Point(point.x - m_x_delta, point.y + m_y_delta))) {
      return true;
    }
  }

  // The comparison described above takes place in the function IsInHole
  return false;
}

}  // namespace fdm