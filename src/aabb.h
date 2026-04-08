#pragma once

#include "interval.h"
#include "ray.h"
#include "vec3.h"
#include <utility>

class aabb { // axis aligned bounding box
  public:
    interval x, y, z;

    aabb() {} // The default AABB is empty, since intervals are empty by default.

    aabb(const interval& x, const interval& y, const interval& z)
      : x(x), y(y), z(z) {}

    aabb(const point3& a, const point3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.

        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    aabb(const aabb& a, const aabb& b) {
        x = interval(a.x, b.x);
        y = interval(a.y, b.y);
        z = interval(a.z, b.z);
    }

    const interval& axis_interval(int n) const {
        switch (n) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: std::unreachable();
        }
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_orig = r.origin();
        const vec3&   ray_dir  = r.direction();

        for (int axis_num = 0; axis_num < 3; axis_num++) {
            const interval& axis = axis_interval(axis_num);
            const float inv_axis_dir = 1.0f / ray_dir[axis_num];

            // x = Qx + t * dx
            // t = (x - Qx) / dx
            // x = intersection point
            // Qx = origin of ray
            // dx = direction of ray
            float t0 = (axis.min - ray_orig[axis_num]) * inv_axis_dir;
            float t1 = (axis.max - ray_orig[axis_num]) * inv_axis_dir;

            // narrow the ray interval to the timeframe where it intersects the axis's interval
            if (t0 < t1) {
                if (t0 > ray_t.min) 
                    ray_t.min = t0;
                if (t1 < ray_t.max) 
                    ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) 
                    ray_t.min = t1;
                if (t0 < ray_t.max) 
                    ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                // the time intervals where the ray intersects each axis intervals do not lineup
                return false;
        }
        return true;
    }
};
