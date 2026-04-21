#pragma once
#include <tiny/optional.h>
#include <memory>
#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

class sphere : public hittable {
public:
    sphere(const point3& center, float radius, const material* mat_ptr) noexcept : center(center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {
    }
    sphere(const point3& center1, const point3& center2, float radius, const material* mat_ptr) noexcept : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {

    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
    aabb bounding_box() const override {
        vec3 radius_vec = vec3(radius, radius, radius);
        const point3& center1 = center.origin();
        point3 center2 = center.at(1);
        aabb bbox0 = aabb(center1 - radius_vec, center1 + radius_vec);
        aabb bbox1 = aabb(center2 - radius_vec, center2 + radius_vec);
        return aabb(bbox0, bbox1);
    }
    static uv get_sphere_uv(const point3& p);
private:
    internal_ray center;
    float radius;
    const material* mat_ptr;
};
