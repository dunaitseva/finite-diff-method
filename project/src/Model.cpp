#include "Model.hpp"

#include <iostream>
#include <tuple>
#include <vector>

#include "CalculationUtils.hpp"

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

void Model::SetOuterRestrictions(
    const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_up,
    const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_down,
    const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_left,
    const restr::BoundaryRestrincionPointerType<ModelNodeType> &restr_right) {
  m_outer_restrictions[restr::UP_RESTRICTION] = restr_up;
  m_outer_restrictions[restr::DOWN_RESTRICTION] = restr_down;
  m_outer_restrictions[restr::LEFT_RESTRICTION] = restr_left;
  m_outer_restrictions[restr::RIGHT_RESTRICTION] = restr_right;
}

void Model::SetOuterRestrictions(
    const restr::BoundaryRestrictionsStorageType<ModelNodeType> &restrictions) {
  m_outer_restrictions = restrictions;
}
void Model::SetInnerRestrictions(
    const restr::BoundaryRestrincionPointerType<ModelNodeType> &restriction) {
  m_inner_restriction = restriction;
}

void Model::TimeIntegrate(
    double total_time, solution::SolutionStorageBase<ModelNodeType> &storage) {
  storage.CommitLayer(m_mesh_ptr_present);

  auto time_integrate_iterations =
      static_cast<size_t>(total_time / m_time_delta);
  // Iterate time layers
  for (size_t t = 0; t < time_integrate_iterations; ++t) {
    m_mesh_ptr_last = m_mesh_ptr_present;
    // Calculate all nodes into plate
    for (size_t j = 1; j < m_mesh_ptr_last->SizeRows() - 1; ++j) {
      for (size_t i = 1; i < m_mesh_ptr_last->SizeCols() - 1; ++i) {
        Point curr_point(static_cast<double>(i) * m_x_delta,
                         static_cast<double>(j) * m_y_delta);
		if (!PointInHole(curr_point)) {
		  if (PointOnBorder(curr_point)) {
			ModelNodeType inner_value = GetInnerNeighbor(i, j);
			m_mesh_ptr_present->SetValue(j, i, m_inner_restriction->operator()(inner_value, m_x_delta));
		  } else {
			equations::HeatConductionParamsType<ModelNodeType>
				equation_parameters{m_mesh_ptr_last->GetValue(j, i),
									m_mesh_ptr_last->GetValue(j, i - 1),
									m_mesh_ptr_last->GetValue(j, i + 1),
									m_mesh_ptr_last->GetValue(j - 1, i),
									m_mesh_ptr_last->GetValue(j + 1, i),
									m_time_delta,
									m_x_delta,
									m_y_delta,
									0.1};
			m_mesh_ptr_present->SetValue(
				j, i, equations::HeatConductionProblem(equation_parameters));
		  }
		}else {
		  m_mesh_ptr_present->SetValue(
			  j, i, 0.0);
		}
      }
    }

	// Calculate boundary nodes
	for (size_t i = 1; i < m_mesh_ptr_last->SizeRows() - 1; ++i) {
	  ModelNodeType T_x_left_inner = m_mesh_ptr_present->GetValue(i, 1);
	  ModelNodeType T_x_right_inner =
		  m_mesh_ptr_present->GetValue(i, m_mesh_ptr_last->SizeCols() - 2);

	  m_mesh_ptr_present->SetValue(
		  i, 0,
		  m_outer_restrictions[restr::LEFT_RESTRICTION]->operator()(
			  T_x_left_inner, m_y_delta));
	  m_mesh_ptr_present->SetValue(
		  i, m_mesh_ptr_last->SizeCols() - 1,
		  m_outer_restrictions[restr::RIGHT_RESTRICTION]->operator()(
			  T_x_right_inner, m_y_delta));
	}
//
//	for (size_t i = 0; i < m_mesh_ptr_last->SizeCols(); ++i) {
//	  ModelNodeType T_x_down_inner = m_mesh_ptr_present->GetValue(1, i);
//	  ModelNodeType T_x_up_inner =
//		  m_mesh_ptr_present->GetValue(m_mesh_ptr_last->SizeRows() - 1, i);
//	  m_mesh_ptr_present->SetValue(
//		  0, i,
//		  m_outer_restrictions[restr::DOWN_RESTRICTION]->operator()(
//			  T_x_down_inner, m_x_delta));
//	  m_mesh_ptr_present->SetValue(
//		  m_mesh_ptr_last->SizeRows() - 1, i,
//		  m_outer_restrictions[restr::UP_RESTRICTION]->operator()(T_x_up_inner,
//																  m_x_delta));
//	}
    storage.CommitLayer(m_mesh_ptr_present);
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

Model::ModelNodeType Model::GetInnerNeighbor(size_t x_shift, size_t y_shift) {
  auto cast_x_shift = static_cast<double>(x_shift);
  auto cast_y_shift = static_cast<double>(y_shift);
  Point up_neighbor((cast_x_shift + 1) * m_x_delta, cast_y_shift * m_y_delta);
  Point down_neighbor((cast_x_shift - 1) * m_x_delta, cast_y_shift * m_y_delta);
  Point right_neighbor(cast_x_shift * m_x_delta,
                       (cast_y_shift - 1) * m_y_delta);
  Point left_neighbor(cast_x_shift * m_x_delta, (cast_y_shift + 1) * m_y_delta);

  if (!PointOnBorder(up_neighbor) && !PointInHole(up_neighbor)) {
    return m_mesh_ptr_last->GetValue(y_shift, x_shift + 1);
  }

  if (!PointOnBorder(down_neighbor) && !PointInHole(down_neighbor)) {
    return m_mesh_ptr_last->GetValue(y_shift, x_shift - 1);
  }

  if (!PointOnBorder(right_neighbor) && !PointInHole(right_neighbor)) {
    return m_mesh_ptr_last->GetValue(y_shift + 1, x_shift);
  }

  if (!PointOnBorder(left_neighbor) && !PointInHole(left_neighbor)) {
    return m_mesh_ptr_last->GetValue(y_shift - 1, x_shift);
  }

  return 0.0;
}

bool Model::PointInHole(Point point) const {
  /*
   * Mathematical part - vector and pseudoscalar product.
   * Implementation - products are considered (1,2,3 - triangle vertices, 0 -
   * point):
   * (x1-x0)*(y2-y1)-(x2-x1)*(y1-y0)
   * (x2-x0)*(y3-y2)-(x3-x2)*(y2-y0)
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