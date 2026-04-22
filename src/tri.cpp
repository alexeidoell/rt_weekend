#include "ray.h"
#include "vec3.h"
#include "vectorization.h"
#include "hittable.h"
#include <tiny/optional.h>
#include "tri.h"

tiny::optional<hit_record> inner_tri::hit(const ray& r, interval ray_t) const noexcept {
    static constexpr hn::CappedTag<float, 4> d;

    // Load stored float arrays directly into SIMD registers
    auto cross = cross_vector_only(u_e, v_e);
    auto len_squared = dot_packed(cross, cross);
    auto normal_v = hn::Mul(cross, hn::ApproximateReciprocalSqrt(hn::Set(d, len_squared)));

    float denom = dot_packed(normal_v, r.direction().vec);
    if (std::fabsf(denom) < 1e-8f)
        return std::nullopt;

    float D = dot_packed(normal_v, anchor);
    float t = (D - dot_packed(normal_v, r.origin().vec)) / denom;
    if (!ray_t.contains(t))
        return std::nullopt;

    point3 hit_point = point_at(r.origin().vec, r.direction().vec, t);
    auto w_v = hn::Sub(hit_point.vec, anchor);

    float d00 = dot_packed(u_e, u_e);
    float d01 = dot_packed(u_e, v_e);
    float d11 = dot_packed(v_e, v_e);
    float wu  = dot_packed(w_v, u_e);
    float wv  = dot_packed(w_v, v_e);
    float inv_denom = 1.0f / (d00 * d11 - d01 * d01);

    // Pack both barycentric cross-products into one 4-wide SIMD multiply
    alignas(16) float top_arr[4] = {d11, -d01, d00, -d01};
    alignas(16) float bot_arr[4] = {wu,   wv,  wv,   wu};
    auto res_v = hn::Mul(
            hn::Mul(
                hn::Load(d, top_arr),
                hn::Load(d, bot_arr)), 
            hn::Set(d, inv_denom));
    hn::Store(res_v, d, top_arr);

    float b1 = (top_arr[0] + top_arr[1]);  // d11*wu - d01*wv
    float b2 = (top_arr[2] + top_arr[3]);  // d00*wv - d01*wu

    if (b1 < 0 || b2 < 0 || b1 + b2 > 1.0f)
        return std::nullopt;

    return tiny::make_optional<hit_record>(hit_point, t, mat_ptr, r, vec3(normal_v), uv{0.0, 0.0});
}

tiny::optional<hit_record> quad::hit(const ray& r, interval ray_t) const noexcept {
    auto hit1 = t1.hit(r, ray_t);
    if (hit1) return hit1;
    return t2.hit(r, ray_t);
}

std::vector<quad> make_box(const point3& a, const point3& b, const material* mat_ptr) {
    static constexpr auto d = vec3::get_tag();
    static const auto dx_mask = hn::FirstN(d, 1);
    static const auto dy_mask = hn::SlideMask1Up(d, dx_mask);
    static const auto dz_mask = hn::SlideMask1Up(d, dy_mask);
    auto mask = a.vec < b.vec;
    float min_arr[3];
    float max_arr[3];

    auto min_vec = hn::IfThenElse(mask, a.vec, b.vec);
    auto max_vec = hn::IfThenElse(mask, b.vec, a.vec);
    auto diff = max_vec - min_vec;
    auto dx = vec3(hn::IfThenElseZero(dx_mask, diff));
    auto dy = vec3(hn::IfThenElseZero(dy_mask, diff));
    auto dz = vec3(hn::IfThenElseZero(dz_mask, diff));

    hn::StoreN(min_vec, d, min_arr, 3);
    hn::StoreN(max_vec, d, max_arr, 3);

    std::vector<quad> result{};

    result.emplace_back(point3(min_arr[0],min_arr[1],max_arr[2]), dx, dy, mat_ptr); // front
    result.emplace_back(point3(max_arr[0],min_arr[1],max_arr[2]), -dz, dy, mat_ptr); // right
    result.emplace_back(point3(max_arr[0],min_arr[1],min_arr[2]), -dx, dy, mat_ptr); // back
    result.emplace_back(point3(min_arr[0],min_arr[1],min_arr[2]), dz, dy, mat_ptr); // left
    result.emplace_back(point3(min_arr[0],max_arr[1],max_arr[2]), dx, -dz, mat_ptr); // top
    result.emplace_back(point3(min_arr[0],min_arr[1],min_arr[2]), dx, dz, mat_ptr); // bottom

    return result;
}
