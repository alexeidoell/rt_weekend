#include "hittable.h"
#include "hittable_list.h"
#include <print>
#include <tiny/optional.h>

void hit_record::set_face_normal(const ray& r, const vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

tiny::optional<hit_record> hittable_list::hit(const ray &r, interval ray_t) const noexcept {
    if (!bbox.hit(r, ray_t)) return std::nullopt;
    tiny::optional<hit_record> rec = std::nullopt;
    float closest = ray_t.max;

    for (hittable* obj : objects) {
        auto temp = obj->hit(r, interval(ray_t.min, closest));
        if (temp) {
            closest = temp->t;
            rec = temp;
        }
    }

    return rec;
}

void bvh_node::init(std::vector<hittable*>& objects, size_t start, size_t end) {
//    int axis = random_int(0, 2);
    int axis = 0;

    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = objects[start];
        right = nullptr;
    } else if (object_span == 2) {
        left = objects[start];
        right = objects[start + 1];
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        auto mid = start + object_span / 2;
        left = node_list.emplace_back(std::make_unique<bvh_node>(objects, start, mid, node_list)).get();
        right = node_list.emplace_back(std::make_unique<bvh_node>(objects, mid, end, node_list)).get();
    }

    bbox = right ? aabb(left->bounding_box(), right->bounding_box()) : left->bounding_box();
}

tiny::optional<hit_record> bvh_node::hit(const ray& r, interval ray_t) const noexcept {
    if (!bbox.hit(r, ray_t)) return std::nullopt;

    auto hit_left = left->hit(r, ray_t);
    if (!right) return hit_left;

    auto hit_right = right->hit(r, interval(ray_t.min, hit_left ? hit_left->t : ray_t.max));
    return hit_right ? hit_right : hit_left;
}


