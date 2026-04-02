#include "ray.h"
#include "vec3.h"
#include <tiny/optional.h>
#include "tri.h"

tiny::optional<hit_record> tri::hit(const ray& r, interval ray_t) const {
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

    const hwy::N_SSE2::CappedTag<float, 4> d;
    if (!hwy::N_SSE2::AllFalse(hwy::N_SSE2::CappedTag<float, 4>(), hwy::N_SSE2::Gt(bary_coords.vec, hwy::N_SSE2::Set(d, 1.0f))) 
            || !hwy::N_SSE2::AllFalse(hwy::N_SSE2::CappedTag<float, 4>(), hwy::N_SSE2::Lt(bary_coords.vec, hwy::N_SSE2::Set(d, 0.0f)))) {
        return std::nullopt;
    }

    return tiny::make_optional<hit_record>(r.at(t), t, mat_ptr, r, normal);
}


point3 tri::barycentric_coords(const point3& point) const {
    static const hwy::N_SSE2::CappedTag<float, 4> d;
    vec3 anchor_to_point = point - anchor;

    float buffer[4];
    buffer[0] = dot(anchor_to_point, u);
    buffer[1] = dot(anchor_to_point, v);
    buffer[2] = buffer[1];
    buffer[3] = buffer[0];
    auto non_const_vec = hwy::N_SSE2::Load(d, buffer);

    auto result_vec = hwy::N_SSE2::Div(Mul(top, non_const_vec), denom_v);
    hwy::N_SSE2::Store(result_vec, d, buffer);
    float v_coord = buffer[0] + buffer[1];
    float w_coord = buffer[2] + buffer[3];

    float u_coord = 1.0 - v_coord - w_coord;

    return point3(u_coord, v_coord, w_coord);
}

tiny::optional<hit_record> quad::hit(const ray& r, interval ray_t) const {
    auto hit1 = t1.hit(r, ray_t);
    if (hit1) {
        return hit1;
    }
    return t2.hit(r, ray_t);
}
