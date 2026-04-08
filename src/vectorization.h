#pragma once
#include "vec3.h"
#include <hwy/highway.h>

float dot_packed(const hn::Vec128<float> u, hn::Vec128<float> v);
point3 point_at(const hn::Vec128<float> u, const hn::Vec128<float> direction, const float t);
