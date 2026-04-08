#pragma once

#include "aabb.h"
#include "hittable.h"
#include "sphere.h"
#include "tri.h"
#include <concepts>
#include <variant>

using hittable_types = std::variant<sphere, tri, quad>;

template<typename T>
concept Hittable = std::assignable_from<hittable_types, T>;

class hittable_list : public hittable {
public:
    std::vector<hittable_types> objects;

    hittable_list() noexcept {}

    void clear() { objects.clear(); }

    hittable_list(std::span<hittable_types> objs) noexcept {
        for (const auto& obj : objs) {
            add(obj);
        }
    }

    template <class Hittable>
    void add(Hittable s) noexcept { 
        objects.push_back(std::move(s));
        bbox = aabb(bbox, objects.back().visit([](auto&& obj) { return obj.bounding_box(); }));
    }

    aabb bounding_box() const override { return bbox; }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;

private:
    aabb bbox;
};

class bvh_node : public hittable {
public:
    bvh_node(hittable_list list) : bvh_node(list, 0, list.objects.size()) {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }

    bvh_node(hittable_list obj_list, size_t start, size_t end) {
        auto objects = obj_list.objects;
        int axis = random_int(0,2);

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = std::make_shared<hittable_list>(std::span(&objects[start], 1));
        } else if (object_span == 2) {
            left = std::make_shared<hittable_list>(std::span(&objects[start], 1));
            right = std::make_shared<hittable_list>(std::span(&objects[start + 1], 1));
        } else {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            auto mid = start + object_span/2;
            left = std::make_shared<bvh_node>(obj_list, start, mid);
            right = std::make_shared<bvh_node>(obj_list, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());

    }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const override {
        if (!bbox.hit(r, ray_t))
            return std::nullopt;

        tiny::optional<hit_record> hit_left = left->hit(r, ray_t);
        tiny::optional<hit_record> hit_right = right->hit(r, interval(ray_t.min, hit_left ? hit_left->t : ray_t.max));

        if (hit_left && hit_right) {
            return hit_left->t < hit_right->t ? hit_left : hit_right;
        } else if (hit_left) {
            return hit_left;
        } else {
            return hit_right;
        }
    }

    aabb bounding_box() const override { return bbox; }

  private:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    aabb bbox;

        static bool box_compare(
        const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index
    ) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare (const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }

};
