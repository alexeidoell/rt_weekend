#pragma once
#include <tiny/optional.h>
#include <memory>
#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

class sphere {
public:
    sphere(const point3& center, float radius, std::shared_ptr<const material> mat_ptr) noexcept : center(center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {
    }
    sphere(const point3& center1, const point3& center2, float radius, std::shared_ptr<const material> mat_ptr) noexcept : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {

    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept ;
    aabb bounding_box() const {
        vec3 radius_vec = vec3(radius, radius, radius);
        const point3& center1 = center.origin();
        point3 center2 = center.at(1);
        aabb bbox0 = aabb(center1 - radius_vec, center1 + radius_vec);
        aabb bbox1 = aabb(center2 - radius_vec, center2 + radius_vec);
        return aabb(bbox0, bbox1);
    }
private:
    ray center;
    float radius;
    std::shared_ptr<const material> mat_ptr;
};
