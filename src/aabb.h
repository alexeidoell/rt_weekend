#pragma once

#include "interval.h"
#include "ray.h"
#include "vec3.h"
#include "common.h"
#include <utility>

class aabb { // axis aligned bounding box
  public:
    hn::Vec128<float> min_vec, max_vec;

    aabb() {
        static hn::CappedTag<float, 4> d;
        min_vec = hn::Set(d, 0.0f);
        max_vec = hn::Set(d, 0.0f);
    }

    aabb(const interval& x, const interval& y, const interval& z) {
        static hn::CappedTag<float, 4> d;
        float min_arr[4] = {x.min, y.min, z.min, 0.0f}; 
        float max_arr[4] = {x.max, y.max, z.max, 0.0f}; 

        min_vec = hn::Load(d, min_arr);
        max_vec = hn::Load(d, max_arr);
        pad_to_minimums();
    }

    aabb(const point3& a, const point3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        auto mask = a.vec < b.vec;

        min_vec = hn::IfThenElse(mask, a.vec, b.vec);
        max_vec = hn::IfThenElse(mask, b.vec, a.vec);
        pad_to_minimums();
    }

    aabb(const aabb& a, const aabb& b) {
        auto min_mask = a.min_vec < b.min_vec;
        auto max_mask = a.max_vec > b.max_vec;

        min_vec = hn::IfThenElse(min_mask, a.min_vec, b.min_vec);
        max_vec = hn::IfThenElse(max_mask, a.max_vec, b.max_vec);
        pad_to_minimums();
    }

    const interval axis_interval(int n) const {
        switch (n) {
            case 0: return interval(min_vec.raw[0], max_vec.raw[0]);
            case 1: return interval(min_vec.raw[1], max_vec.raw[1]);
            case 2: return interval(min_vec.raw[2], max_vec.raw[2]);
            default: std::unreachable();
        }
    }

    void pad_to_minimums() {
        auto sizes = hn::Sub(max_vec, min_vec);
        const hn::CappedTag<float, 4> d;
        const auto deltas = hn::Set(d, 0.001f);
        auto mask = sizes < deltas;

        min_vec = MaskedSubOr(min_vec, mask, deltas, min_vec);
        max_vec = MaskedAddOr(max_vec, mask, deltas, max_vec);
    }

    int longest_axis() const {
        auto sizes = hn::Sub(max_vec, min_vec);
        if (sizes.raw[0] > sizes.raw[1] && sizes.raw[0] > sizes.raw[2]) return 0;
        if (sizes.raw[1] > sizes.raw[2]) return 1;
        return 2;
    }

      /*
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
    */


    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_orig = r.origin();
        const vec3&   ray_dir  = r.direction();

        auto tmp_t0_vec = (min_vec - ray_orig.vec) * hn::ApproximateReciprocal(ray_dir.vec);
        auto tmp_t1_vec = (max_vec - ray_orig.vec) * hn::ApproximateReciprocal(ray_dir.vec);
        auto mask = tmp_t0_vec < tmp_t1_vec;
        auto t0_vec = hn::IfThenElse(mask, tmp_t0_vec, tmp_t1_vec);
        auto t1_vec = hn::IfThenElse(mask, tmp_t1_vec, tmp_t0_vec);

        for (int axis_num = 0; axis_num < 3; axis_num++) {
            /*
            const interval axis = axis_interval(axis_num);
            const float inv_axis_dir = 1.0f / ray_dir[axis_num];

            // x = Qx + t * dx
            // t = (x - Qx) / dx
            // x = intersection point
            // Qx = origin of ray
            // dx = direction of ray
            float t0 = (axis.min - ray_orig[axis_num]) * inv_axis_dir;
            float t1 = (axis.max - ray_orig[axis_num]) * inv_axis_dir;
            */

            float t0 = t0_vec.raw[axis_num];
            float t1 = t1_vec.raw[axis_num];

            // narrow the ray interval to the timeframe where it intersects the axis's interval
            if (t0 > ray_t.min) 
                ray_t.min = t0;
            if (t1 < ray_t.max) 
                ray_t.max = t1;

            if (ray_t.max <= ray_t.min)
                // the time intervals where the ray intersects each axis intervals do not lineup
                return false;
        }
        return true;
    }
};

namespace const_aabb {
    static const aabb empty = aabb(const_interval::empty, const_interval::empty, const_interval::empty);
    static const aabb universe = aabb(const_interval::universe, const_interval::universe, const_interval::universe);
}
