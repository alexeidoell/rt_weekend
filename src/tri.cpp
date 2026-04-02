#include "ray.h"
#include "vec3.h"
#include <tiny/optional.h>
#include "tri.h"

tiny::optional<hit_record> tri::hit(const ray& r, interval ray_t) const noexcept {
    float denominator = dot(normal, r.direction());
    if (std::fabs(denominator) < 1e-8) { // parallel to plane
        return std::nullopt;
    }
    float t = (D - dot(normal, r.origin())) / denominator;

    if (!ray_t.contains(t)) {
        return std::nullopt;
    }

    point3 point = r.at(t);
    point3 bary_coords = barycentric_coords(point);

    if (bary_coords.x() < 0 || bary_coords.x() > 1 ||
            bary_coords.y() < 0 || bary_coords.y() > 1 ||
            bary_coords.z() < 0 || bary_coords.z() > 1) {
        return std::nullopt;
    }

    return tiny::make_optional<hit_record>(r.at(t), t, mat_ptr, r, normal);
}


point3 tri::barycentric_coords(const point3& point) const {
    vec3 atp = point - anchor;
    float dot_u = dot(atp, u);
    float dot_v_coord = dot(atp, v);
    float inv_denom = 1.0f / denominator;
    float v_coord = (d11 * dot_u - d01 * dot_v_coord) * inv_denom;
    float w_coord = (d00 * dot_v_coord - d01 * dot_u) * inv_denom;
    float u_coord = 1.0f - v_coord - w_coord;
    return point3(u_coord, v_coord, w_coord);
}

tiny::optional<hit_record> quad::hit(const ray& r, interval ray_t) const noexcept {
    auto hit1 = t1.hit(r, ray_t);
    if (hit1) {
        return hit1;
    }
    return t2.hit(r, ray_t);
}
