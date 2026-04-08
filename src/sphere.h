#pragma once
#include <tiny/optional.h>
#include <memory>
#include "hittable.h"
#include "vec3.h"

class sphere {
public:
    sphere(const point3& center, float radius, std::shared_ptr<const material> mat_ptr) noexcept : center(center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {}
    sphere(const point3& center1, const point3& center2, float radius, std::shared_ptr<const material> mat_ptr) noexcept : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {}
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept;
private:
    ray center;
    float radius;
    std::shared_ptr<const material> mat_ptr;
};
