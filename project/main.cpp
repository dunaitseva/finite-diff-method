#include <iostream>
#include <iomanip>

#include "Model.hpp"

int main() {
  fdm::Model model(7.0, 5.0, 0.2, 0.1);
  model.SetInitialCondition(11);

  model.SetHoleGeometry(fdm::Model::Point(2.0, 1.0), fdm::Model::Point(5.0, 1.0), fdm::Model::Point(5.0, 3.0));
  fdm::solution::StandardStreamStorage<fdm::Model::ModelNodeType> stream_storage;
  model.TimeIntegrate(1.0, stream_storage);
  return 0;
}