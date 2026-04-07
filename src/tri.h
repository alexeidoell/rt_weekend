#pragma once

#include "hittable.h"
#include "vec3.h"
#include <tiny/optional.h>

class tri {
public:
    tri(const point3& p1, const point3& p2, const point3& p3, std::shared_ptr<const material> mat_ptr) noexcept {
        vec3 u_vec = p2 - p1;
        vec3 v_vec = p3 - p1;
        vec3 n = unit_vector(cross(u_vec, v_vec));
        anchor = p1.vec;
        u_e = u_vec.vec;
        v_e = v_vec.vec;
        this->mat_ptr = mat_ptr;
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept;
private:
    hwy::N_AVX2::Vec128<float> anchor;
    hwy::N_AVX2::Vec128<float> u_e;
    hwy::N_AVX2::Vec128<float> v_e;
    std::shared_ptr<const material> mat_ptr;
};

class quad {
public:
    quad(const point3& p1, const vec3& u, const vec3& v, std::shared_ptr<const material> mat_ptr) noexcept : t1(p1, p1 + u, p1 + v, mat_ptr), t2(p1 + u + v, p1 + u, p1 + v, mat_ptr) {}
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept;
private:
    tri t1, t2;
};
