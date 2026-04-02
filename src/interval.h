#pragma once

#include "common.h"
class interval {
public:
    const float min, max;

    constexpr interval() : min(-infinity), max(infinity) {}
    constexpr interval(float min, float max) : min(min), max(max) {}

    float size() const {
        return max - min;
    }

    bool contains(float x) const {
        return x >= min && x <= max;
    }

    bool surrounds(float x) const {
        return x > min && x < max;
    }

    float clamp(float x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

};

constexpr interval empty = interval(infinity, -infinity);
constexpr interval universe = interval(-infinity, infinity);
