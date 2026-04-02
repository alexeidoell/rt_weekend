#pragma once


#include "vec3.h"
#include "vectorization.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction) {}

    const point3& origin() const { return orig; }
    const vec3& direction() const { return dir; }

    vec3 at(double t) const {
        return N_SSE2::point_at(orig.vec, dir.vec, t);
    }

private:
    point3 orig;
    vec3 dir;
};
