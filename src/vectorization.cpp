#include "vectorization.h"
#include "vec3.h"

float HWY_STATIC_NAMESPACE::dot_packed(const hwy::HWY_STATIC_NAMESPACE::Vec128<float> u, hwy::HWY_STATIC_NAMESPACE::Vec128<float> v) {
  const hwy::HWY_STATIC_NAMESPACE::CappedTag<float, 4> d;
  auto result = Mul(u, v);
  return hwy::HWY_STATIC_NAMESPACE::ReduceSum(d, result);
}

point3 HWY_STATIC_NAMESPACE::point_at(const hwy::HWY_STATIC_NAMESPACE::Vec128<float> u, const hwy::HWY_STATIC_NAMESPACE::Vec128<float> direction, const float t) {
  const hwy::HWY_STATIC_NAMESPACE::CappedTag<float, 4> d;
  auto result_vec = MulAdd(direction, Set(d, t), u);
  return point3(result_vec);
}
