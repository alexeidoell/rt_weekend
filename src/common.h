#pragma once

#include <random>
#include "pcg_random.hpp"

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
    static pcg32 generator(seed_source);
    return distribution(generator);
}

inline float random_double(float min, float max) {
    return min + (max - min) * random_double();
}
