#ifndef FINITEDIFFERENCEMETHOD_CALCULATIONUTILS_HPP_
#define FINITEDIFFERENCEMETHOD_CALCULATIONUTILS_HPP_

#include <memory>
#include <tuple>

namespace fdm {
/*
 * I decided to make the restrictions separate from the model class,
 * since if desired, these classes can be reused for other specific models.
 */
namespace restr {
template <typename ModelNodeType>
class BaseRestriction {
 public:
  virtual ModelNodeType operator()(ModelNodeType, double delta) = 0;
  virtual ~BaseRestriction() = default;
};

/**
 * As we know, first kind restriction return constant value not depend
 * on boundary derivative value.
 */
template <typename ModelNodeType>
class FirstKindRestriction : public BaseRestriction<ModelNodeType> {
 public:
  explicit FirstKindRestriction(double constant = 0.0) : m_constant(constant) {}
  ModelNodeType operator()(ModelNodeType inner, double delta) override {
    return m_constant;
  }

 private:
  double m_constant;
};

template <typename ModelNodeType>
class SecondKindRestriction : public BaseRestriction<ModelNodeType> {
 public:
  explicit SecondKindRestriction(double constant = 0.0)
      : m_constant(constant) {}
  ModelNodeType operator()(ModelNodeType inner, double delta) override {
    return inner - m_constant * delta;
  }

 private:
  double m_constant;
};

template <typename ModelNodeType>
class ThirdKindRestriction : public BaseRestriction<ModelNodeType> {
 public:
  ModelNodeType operator()(ModelNodeType inner, double delta) override {
    return inner / (1 + delta);
  }
};

/*
 * Restriction is the callable object, that calculate boundary values.
 * Parameters are passed to the function, in the bellow signature,
 * in order of taking the derivative from inside to outside. Last parameter
 * represent differential specific derivative increment value.
 */
template <typename ModelNodeType>
using BoundaryRestrincionType = BaseRestriction<ModelNodeType>;
template <typename ModelNodeType>
using BoundaryRestrincionPointerType =
    std::shared_ptr<BoundaryRestrincionType<ModelNodeType>>;
// Restrictions on the outer boundaries of tube.
template <typename ModelNodeType>
using BoundaryRestrictionsStorageType =
    std::array<BoundaryRestrincionPointerType<ModelNodeType>, 4>;
// Order of restrictions
constexpr size_t UP_RESTRICTION = 0;
constexpr size_t DOWN_RESTRICTION = 1;
constexpr size_t LEFT_RESTRICTION = 2;
constexpr size_t RIGHT_RESTRICTION = 3;
}  // namespace restr

namespace equations {
/**
 * @param params pass tuple of T_i_j, T_i-1_j, T_i+1,j, T_i_j-1, T_i_j+1, delta
 * t, delta x, delta y and a
 */
template <typename ModelNodeType>
using HeatConductionParamsType =
    std::tuple<ModelNodeType, ModelNodeType, ModelNodeType, ModelNodeType,
               ModelNodeType, double, double, double, double>;
/**
 * Solve the non-stationary problem of heat conduction by an explicit method.
 * @tparam ModelNodeType model node type
 * @param params pass tuple of T_i_j, T_i-1_j, T_i+1,j, T_i_j-1, T_i_j+1, delta
 * t, delta x, delta y and a
 * @return predict node T_i_j value on new time layer
 */
template <typename ModelNodeType>
ModelNodeType HeatConductionProblem(
    const HeatConductionParamsType<ModelNodeType> &params) {
  auto [T_curr, T_x_past, T_x_next, T_y_past, T_y_next, dt, dx, dy, a] = params;
  double dx2 = dx * dx;
  double dy2 = dy * dy;
  ModelNodeType Dx = (T_x_next - 2 * T_curr + T_x_past) / dx2;
  ModelNodeType Dy = (T_y_next - 2 * T_curr + T_y_past) / dy2;
  ModelNodeType D = Dx + Dy;
  return a * dt * D + T_curr;
}
}  // namespace equations

}  // namespace fdm

#endif  // FINITEDIFFERENCEMETHOD_CALCULATIONUTILS_HPP_
