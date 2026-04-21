#pragma once

#include "common.h"
class interval {
public:
    float min, max;

    constexpr interval() : min(-infinity), max(infinity) {}
    constexpr interval(float min, float max) : min(min), max(max) {}

    float size() const { return max - min; }
    bool contains(float x) const { return x >= min && x <= max; }
    bool surrounds(float x) const { return x > min && x < max; }
    float clamp(float x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    interval expand(float delta) const {
        delta = delta/2;
        return interval(min - delta, max + delta);
    }

    interval(const interval& a, const interval& b) {
        min = a.min < b.min ? a.min : b.min;
        max = a.max > b.max ? a.max : b.max;
    }
};

namespace const_interval {
    static constexpr interval empty = interval(infinity, -infinity);
    static constexpr interval universe = interval(-infinity, infinity);
};
