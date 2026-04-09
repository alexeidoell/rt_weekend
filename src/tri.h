#pragma once

#include "aabb.h"
#include "hittable.h"
#include "vec3.h"
#include <memory>
#include <tiny/optional.h>


class inner_tri {
public:
    inner_tri(const point3& p1, const point3& p2, const point3& p3, std::shared_ptr<const material> mat_ptr) noexcept {
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
    hn::Vec128<float> anchor;
    hn::Vec128<float> u_e;
    hn::Vec128<float> v_e;
    std::shared_ptr<const material> mat_ptr;
};

class tri : public hittable {
public:
    tri(const point3& p1, const point3& p2, const point3& p3, std::shared_ptr<const material> mat_ptr) noexcept : inner(p1, p2, p3, mat_ptr) {
        bbox = aabb(aabb(p1, p2), aabb(p2, p3));
    }
    aabb bounding_box() const override {
        return bbox;
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override {
            return inner.hit(r, ray_t);
    }
    inner_tri inner;
    aabb bbox;
};

class quad : public hittable {
public:
    quad(const point3& p1, const vec3& u, const vec3& v, std::shared_ptr<const material> mat_ptr) noexcept : t1(p1, p1 + u, p1 + v, mat_ptr), t2(p1 + u + v, p1 + u, p1 + v, mat_ptr) {
        auto bbox_diagonal1 = aabb(p1, p1 + u + v);
        auto bbox_diagonal2 = aabb(p1 + u, p1 + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }
    aabb bounding_box() const override {
        return bbox;
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
private:
    inner_tri t1, t2;
    aabb bbox;
};
