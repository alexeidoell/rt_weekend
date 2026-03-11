#pragma once

#include "interval.h"
#include "vec3.h"
#include <iostream>

using color3 = vec3;

inline void write_color(std::ostream& out, const color3& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    static const interval intensity(0.000, 0.999);

    // Translate the [0,1] component values to the byte range [0,255].
    int rbyte = int(255.0 * intensity.clamp(r));
    int gbyte = int(255.0 * intensity.clamp(g));
    int bbyte = int(255.0 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

