#pragma once
#include "vec3.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {

float dot_packed(const hwy::HWY_STATIC_NAMESPACE::Vec128<float> u, hwy::HWY_STATIC_NAMESPACE::Vec128<float> v);
point3 point_at(const hwy::HWY_STATIC_NAMESPACE::Vec128<float> u, const hwy::HWY_STATIC_NAMESPACE::Vec128<float> direction, const float t);

}
HWY_AFTER_NAMESPACE();
