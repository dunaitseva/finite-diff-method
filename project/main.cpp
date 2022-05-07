#include <iomanip>
#include <iostream>
#include <memory>

#include "Model.hpp"

int main() {
  fdm::Model model(6.0, 4.0, 0.3, 0.1);
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

  fdm::solution::StandardStreamStorage<fdm::Model::ModelNodeType>
      stream_storage;

  model.TimeIntegrate(25.0, stream_storage);
  return 0;
}