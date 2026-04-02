#include "ray.h"
#include "vec3.h"
#include <tiny/optional.h>
#include "tri.h"

tiny::optional<hit_record> tri::hit(const ray& r, interval ray_t) const {
    double denominator = dot(normal, r.direction());
    if (std::fabs(denominator) < 1e-8) { // parallel to plane
        return std::nullopt;
    }
    double t = (D - dot(normal, r.origin())) / denominator;

    if (!ray_t.contains(t)) {
        return std::nullopt;
    }

    point3 point = r.at(t);
    point3 bary_coords = barycentric_coords(point);

    if (bary_coords.x() > 1 || bary_coords.x() < 0 || bary_coords.y() > 1 || bary_coords.y() < 0 || bary_coords.z() > 1 || bary_coords.z() < 0) {
        return std::nullopt;
    }

    return tiny::make_optional<hit_record>(r.at(t), t, mat_ptr, r, normal);
}


point3 tri::barycentric_coords(const point3& point) const {
    vec3 anchor_to_point = point - anchor;

    double d00 = dot(u, u);
    double d01 = dot(u, v);
    double d11 = dot(v, v);
    double d20 = dot(anchor_to_point, u);
    double d21 = dot(anchor_to_point, v);

    double denominator = d00 * d11 - d01 * d01;
    double v_coord = (d11 * d20 - d01 * d21) / denominator;
    double w_coord = (d00 * d21 - d01 * d20) / denominator;
    double u_coord = 1.0 - v_coord - w_coord;

    return point3(u_coord, v_coord, w_coord);
}

tiny::optional<hit_record> quad::hit(const ray& r, interval ray_t) const {
    auto hit1 = t1.hit(r, ray_t);
    if (hit1) {
        return hit1;
    }
    return t2.hit(r, ray_t);
}
