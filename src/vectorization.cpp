#include "vectorization.h"
#include "vec3.h"

float N_SSE2::dot_packed(const hwy::N_SSE2::Vec128<float> u, hwy::N_SSE2::Vec128<float> v) {
  hwy::N_SSE2::CappedTag<float, 4> d;
  auto result = Mul(u, v);
  return hwy::N_SSE2::ReduceSum(d, result);
}

/*
float N_SSE2::dot_packed(const float u[3], const float v[3]) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}
*/

point3 N_SSE2::point_at(const hwy::N_SSE2::Vec128<float> u, const hwy::N_SSE2::Vec128<float> direction, const float t) {
  hwy::N_SSE2::CappedTag<float, 4> d;
  auto result_vec = MulAdd(direction, Set(d, t), u);
  return point3(result_vec);
}
