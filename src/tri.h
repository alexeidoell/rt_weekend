#pragma once

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include <memory>
#include <tiny/optional.h>


class inner_tri {
public:
    inner_tri(const point3& p1, const point3& p2, const point3& p3, const material* mat_ptr) noexcept {
        vec3 u_vec = p2 - p1;
        vec3 v_vec = p3 - p1;
        vec3 n = unit_vector(cross(u_vec, v_vec));
        anchor = p1.vec;
        u_e = u_vec.vec;
        v_e = v_vec.vec;
        this->mat_ptr = mat_ptr;
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept;

    hn::Vec128<float> anchor;
    hn::Vec128<float> u_e;
    hn::Vec128<float> v_e;
private:
    const material* mat_ptr;
};

class tri : public hittable {
public:
    tri(const point3& p1, const point3& p2, const point3& p3, const material* mat_ptr) noexcept : inner(p1, p2, p3, mat_ptr) {
    }
    aabb bounding_box() const override {
        return aabb(aabb(inner.anchor, inner.u_e), aabb(inner.u_e, inner.v_e));
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override {
            return inner.hit(r, ray_t);
    }
    inner_tri inner;
};

class quad : public hittable {
public:
    quad(const point3& p1, const vec3& u, const vec3& v, const material* mat_ptr) noexcept : t1(p1, p1 + u, p1 + v, mat_ptr), t2(p1 + u + v, p1 + u, p1 + v, mat_ptr) {
    }
    aabb bounding_box() const override {

        auto bbox_diagonal1 = aabb(aabb(t1.anchor, t1.u_e), aabb(t1.u_e, t1.v_e));
        auto bbox_diagonal2 = aabb(aabb(t2.anchor, t2.u_e), aabb(t2.u_e, t2.v_e));
        return aabb(bbox_diagonal1, bbox_diagonal2);
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
private:
    inner_tri t1, t2;
};

std::vector<quad> make_box(const point3& a, const point3& b, const material* mat_ptr);
