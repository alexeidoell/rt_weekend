#pragma once

#include "common.h"
class interval {
public:
    const double min, max;

    constexpr interval() : min(-infinity), max(infinity) {}
    constexpr interval(double min, double max) : min(min), max(max) {}

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return x >= min && x <= max;
    }

    bool surrounds(double x) const {
        return x > min && x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

};

constexpr interval empty = interval(infinity, -infinity);
constexpr interval universe = interval(-infinity, infinity);
