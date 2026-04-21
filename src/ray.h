#pragma once

#include "common.h"
#include "vec3.h"
#include "vectorization.h"

class internal_ray {
    public:
        internal_ray() {}
        internal_ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction) {}

        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        vec3 at(float t) const {
            return point_at(orig.vec, dir.vec, t);
        }
    private:
        point3 orig;
        vec3 dir;
};

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction, float time = 0.f)
        : inner(origin, direction), tm(time) {}

    const point3& origin() const {
        return inner.origin();
    }

    const vec3& direction() const {
        return inner.direction();
    }

    float time() const {
        return tm;
    }

    vec3 at(float t) const {
        return inner.at(t);
    }

private:
    internal_ray inner;
    float tm;
};
