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
    }    return tiny::make_optional<hit_record>(r.at(t), t, mat_ptr, r, normal);
}


point3 tri::barycentric_coords(const point3& point) const {
    static constexpr hwy::HWY_STATIC_NAMESPACE::CappedTag<float, 4> d;
    vec3 anchor_to_point = point - anchor;

    float buffer[4];
    buffer[0] = dot(anchor_to_point, u);
    buffer[1] = dot(anchor_to_point, v);
    buffer[2] = buffer[1];
    buffer[3] = buffer[0];
    auto non_const_vec = hwy::HWY_STATIC_NAMESPACE::Load(d, buffer);

    auto result_vec = hwy::HWY_STATIC_NAMESPACE::Div(Mul(top, non_const_vec), denom_v);
    hwy::HWY_STATIC_NAMESPACE::Store(result_vec, d, buffer);
    float v_coord = buffer[0] + buffer[1];
    float w_coord = buffer[2] + buffer[3];

    float u_coord = 1.0 - v_coord - w_coord;

    return point3(u_coord, v_coord, w_coord);
}

tiny::optional<hit_record> quad::hit(const ray& r, interval ray_t) const noexcept {
    auto hit1 = t1.hit(r, ray_t);
    if (hit1) {
        return hit1;
    }
    return t2.hit(r, ray_t);
}
