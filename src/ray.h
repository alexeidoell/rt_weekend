#pragma once

#include "common.h"
#include "vec3.h"
#include "vectorization.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction, float time)
        : orig(origin), dir(direction), tm(time) {}

    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction) {}

    const point3& origin() const { return orig; }
    const vec3& direction() const { return dir; }

    float time() const {
        return tm;
    }

    vec3 at(float t) const {
        return point_at(orig.vec, dir.vec, t);
    }

private:
    float tm;
    point3 orig;
    vec3 dir;
};
