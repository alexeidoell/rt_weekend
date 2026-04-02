#pragma once
#include <tiny/optional.h>
#include "hittable.h"
#include "vec3.h"
#include <memory>

class sphere : public hittable {
public:
    sphere(const point3& center, float radius, std::shared_ptr<material> mat_ptr) : center(center), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {}
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override;
private:
    point3 center;
    float radius;
    std::shared_ptr<material> mat_ptr;
};
