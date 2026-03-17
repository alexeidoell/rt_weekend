#pragma once


#include "hittable.h"
#include "vec3.h"
#include <memory>
#include "../tiny-optional/include/tiny/optional.h"

class sphere : public hittable {
public:
    sphere(const point3& center, double radius, std::shared_ptr<material> mat_ptr) : center(center), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {}
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override;
private:
    point3 center;
    double radius;
    std::shared_ptr<material> mat_ptr;
};
