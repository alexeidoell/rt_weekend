#pragma once

#include <random>
#include <hwy/highway.h>
#include "pcg_random.hpp"

namespace hn = hwy::HWY_STATIC_NAMESPACE;

// Constants

constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932385;

// Utility Functions

inline constexpr float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

inline float random_double() {
    static pcg_extras::seed_seq_from<std::random_device> seed_source;
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    thread_local static pcg32 generator(seed_source);
    return distribution(generator);
}

inline float random_double(float min, float max) {
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max + 1));
}

