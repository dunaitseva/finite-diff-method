#include <iomanip>
#include <iostream>
#include <memory>

#include "Model.hpp"

constexpr double n_delta = 0.3;
constexpr double t_delta = 0.07;
constexpr double scale = 1;
constexpr double final_n_delta = n_delta * scale;
constexpr double final_t_delta = t_delta * scale;

int main() {
  std::cout << "Computing started with values:" << std::endl;
  std::cout << "dx = dy = " << final_n_delta << std::endl;
  std::cout << "dt = " << final_t_delta << std::endl;

  fdm::Model model(6.0, 4.0, final_n_delta, final_t_delta);
  model.SetInitialCondition(20);

  fdm::restr::BoundaryRestrictionsStorageType<fdm::Model::ModelNodeType>
      restrictions;
  restrictions[fdm::restr::UP_RESTRICTION] = std::make_shared<
      fdm::restr::FirstKindRestriction<fdm::Model::ModelNodeType>>(20.0);
  restrictions[fdm::restr::DOWN_RESTRICTION] = std::make_shared<
      fdm::restr::SecondKindRestriction<fdm::Model::ModelNodeType>>(40.0);
  restrictions[fdm::restr::LEFT_RESTRICTION] = std::make_shared<
      fdm::restr::SecondKindRestriction<fdm::Model::ModelNodeType>>(40.0);
  restrictions[fdm::restr::RIGHT_RESTRICTION] = std::make_shared<
      fdm::restr::SecondKindRestriction<fdm::Model::ModelNodeType>>(40.0);
  fdm::restr::BoundaryRestrincionPointerType<fdm::Model::ModelNodeType>
      inner_restr = std::make_shared<
          fdm::restr::ThirdKindRestriction<fdm::Model::ModelNodeType>>();

  model.SetOuterRestrictions(restrictions);
  model.SetInnerRestrictions(inner_restr);
  model.SetHoleGeometry(fdm::Model::Point(2.0, 1.0),
                        fdm::Model::Point(5.0, 1.0),
                        fdm::Model::Point(5.0, 3.0));

  fdm::solution::PlaceholderStorage<fdm::Model::ModelNodeType>
      stream_storage_placeholder;
  fdm::solution::StaticGnuplotHeatmapStorage<fdm::Model::ModelNodeType>
	  stream_storage_gnuplot("heatmap");

  model.TimeIntegrate(25.0, stream_storage_placeholder, 0);
  model.SaveResult(stream_storage_gnuplot);
  return 0;
}