#pragma once
#include "vec3.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

float dot_packed(const hwy::N_SSE2::Vec128<float> u, hwy::N_SSE2::Vec128<float> v);
point3 point_at(const hwy::N_SSE2::Vec128<float> u, const hwy::N_SSE2::Vec128<float> direction, const float t);
}
HWY_AFTER_NAMESPACE();
