#pragma once

#include "hittable.h"
#include "vec3.h"
#include <memory>
#include <tiny/optional.h>

class tri : public hittable {
public:
    tri(const point3& p1, const point3& p2, const point3& p3, std::shared_ptr<material> mat_ptr) : anchor(p1), p2(p2), p3(p3), mat_ptr(mat_ptr) {
        u = p2 - anchor;
        v = p3 - anchor;
        normal = unit_vector(cross(u, v));

        D = dot(normal, anchor);
    }
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override;
private:
    point3 anchor, p2, p3;
    vec3 u, v;
    vec3 normal;
    double D;
    std::shared_ptr<material> mat_ptr;

    point3 barycentric_coords(const point3& point) const;
};

class quad : public hittable {
public:
    quad(const point3& p1, const vec3& u, const vec3& v, std::shared_ptr<material> mat_ptr) : t1(p1, p1 + u, p1 + v, mat_ptr), t2(p1 + u + v, p1 + u, p1 + v, mat_ptr) {}
    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override;
private:
    tri t1, t2;
};
