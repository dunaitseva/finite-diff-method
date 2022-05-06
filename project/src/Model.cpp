#include "Model.hpp"

namespace fdm {
Model::Model(double width, double height, size_t nodes_x, size_t nodes_y,
             double total_time, double time_delta)
    : m_mesh_ptr(MatrixBuilder().Build<ModelNodeType>()),
      m_width(width),
      m_height(height),
      m_nodes_x(nodes_x),
      m_nodes_y(nodes_y),
      m_total_time(total_time),
      m_time_delta(time_delta) {
  m_mesh_ptr->SetSize(m_nodes_y, m_nodes_x);
  m_x_delta = m_width / static_cast<double>(m_nodes_x);
  m_y_delta = m_height / static_cast<double>(m_nodes_y);
  m_time_iterations = static_cast<size_t>(m_total_time / m_time_delta);

  m_hole_geometry[0] = Point();
  m_hole_geometry[1] = Point();
  m_hole_geometry[2] = Point();
}

void Model::SetHoleGeometry(Point p1, Point p2, Point p3) {
  m_hole_geometry[0] = p1;
  m_hole_geometry[1] = p2;
  m_hole_geometry[2] = p3;
}

}  // namespace fdm