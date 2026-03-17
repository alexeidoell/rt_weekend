#pragma once

#include "ray.h"
#include "interval.h"
#include "vec3.h"
#include <memory>
#include <optional>
#include "../tiny-optional/include/tiny/optional.h"

class material;

class hit_record { 
public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    std::shared_ptr<material> mat_ptr;

    hit_record(const point3 p, const double t, const std::shared_ptr<material> mat_ptr, const ray& hit_ray, const vec3& outward_normal) : p(p), t(t), mat_ptr(mat_ptr) { 
        set_face_normal(hit_ray, outward_normal);
    }


    void set_face_normal(const ray& r, const vec3& outward_normal);
};

class hittable {
public:
    virtual ~hittable() = default;
    virtual tiny::optional<hit_record> hit(const ray& r, interval ray_t) const = 0;
};

class hittable_list : public hittable {
public:
    std::vector<std::unique_ptr<hittable>> objects;

    hittable_list() {}

    void clear(); 

    void add(std::unique_ptr<hittable> &&object);

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override;
};
