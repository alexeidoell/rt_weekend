#pragma once

#include "ray.h"
#include "interval.h"
#include "vec3.h"
#include <tiny/optional.h>

class material;

class hit_record { 
public:
    point3 p;
    vec3 normal;
    float t;
    bool front_face;
    const material* mat_ptr; // cannot be a reference because it deletes the copy constructor of optional<hit_record>

    hit_record(const point3 p, const float t, const material* mat_ptr, const ray& hit_ray, const vec3& outward_normal) : p(p), t(t), mat_ptr(mat_ptr) { 
        set_face_normal(hit_ray, outward_normal);
    }

    void set_face_normal(const ray& r, const vec3& outward_normal);
};

class hittable {
public:
    virtual ~hittable() = default;
    virtual tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept = 0;
};
