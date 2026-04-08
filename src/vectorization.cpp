#include "vectorization.h"
#include "vec3.h"

float dot_packed(const hn::Vec128<float> u, hn::Vec128<float> v) {
  const hn::CappedTag<float, 4> d;
  auto result = Mul(u, v);
  return hn::ReduceSum(d, result);
}

point3 point_at(const hn::Vec128<float> u, const hn::Vec128<float> direction, const float t) {
  const hn::CappedTag<float, 4> d;
  auto result_vec = MulAdd(direction, Set(d, t), u);
  return point3(result_vec);
}
