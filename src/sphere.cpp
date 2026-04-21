#include "sphere.h"
#include "vec3.h"

tiny::optional<hit_record> sphere::hit(const ray& r, interval ray_t) const noexcept {
    point3 current_center = center.at(r.time());
    vec3 oc = current_center - r.origin();
    float a = r.direction().length_squared();
    float h = dot(r.direction(), oc);
    float c = oc.length_squared() - radius*radius;

    float discriminant = h*h - a*c;
    if (discriminant < 0)
        return std::nullopt;

    float sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            return std::nullopt;
        }
    }

    point3 hit_point = r.at(root);
    vec3 outward_normal = (hit_point - current_center) / radius;

    return tiny::make_optional<hit_record>(hit_point, root, mat_ptr, r, outward_normal);
}

