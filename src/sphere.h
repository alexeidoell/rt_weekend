#pragma once
#include <tiny/optional.h>
#include "hittable.h"
#include "vec3.h"

class sphere {
public:
    sphere(const point3& center, float radius, std::shared_ptr<const material> mat_ptr) noexcept : center(center), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {}
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept;
private:
    point3 center;
    float radius;
    std::shared_ptr<const material> mat_ptr;
};
