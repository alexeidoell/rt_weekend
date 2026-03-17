#pragma once


#include "hittable.h"
#include "vec3.h"
#include <memory>
#include "../tiny-optional/include/tiny/optional.h"

class sphere : public hittable {
public:
    sphere(const point3& center, double radius, std::shared_ptr<material> mat_ptr) : center(center), radius(std::fmax(0,radius)), mat_ptr(mat_ptr) {}

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;


        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return std::nullopt;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return std::nullopt;
            }
        }

        point3 hit_point = r.at(root);
        vec3 outward_normal = (hit_point - center) / radius;

        return tiny::make_optional<hit_record>(hit_point, root, mat_ptr, r, outward_normal);
    }

private:
    point3 center;
    double radius;
    std::shared_ptr<material> mat_ptr;
};
