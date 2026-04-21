#include "sphere.h"
#include "vec3.h"
#include <cmath>

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
    uv rec_uv = get_sphere_uv(outward_normal);

    return tiny::make_optional<hit_record>(hit_point, root, mat_ptr, r, outward_normal, rec_uv);
}


static uv get_sphere_uv(const point3& p) {
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    float theta = std::acosf(-p.y());
    float phi = std::atan2f(-p.z(), p.x()) + pi;

    return uv{phi / (2 * pi), theta / pi};
}
