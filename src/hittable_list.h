#pragma once

#include "aabb.h"
#include "hittable.h"
#include "sphere.h"
#include "tri.h"
#include <tuple>
#include <utility>
#include <variant>
#include <vector>
#include <algorithm>
#include <tiny/optional.h>

template <typename T>
concept hittable_concept = requires(T a, ray r, interval ray_t) {
    { a.hit(r, ray_t) } -> std::same_as<tiny::optional<hit_record>>;
    { a.bounding_box() } -> std::same_as<aabb>;
};

template<hittable_concept... hittables>
class hittable_list : hittable {
public:
    std::vector<std::variant<hittables*...>> objects; // raw ptrs into above; populated by build()
    std::tuple<std::vector<hittables>...> data;

    hittable_list() noexcept {}

    template<typename T, typename... Params>
    void add(Params... args) noexcept { 
        std::get<std::vector<T>>(data).emplace_back(args...);
    }

    // Call after all adds. Populates objects[] and recomputes bbox.
    void build() noexcept {
        std::apply([&](auto&... vecs) {
                (build_one(vecs), ...);
                }, data);
    }

    template<typename T>
    void build_one(std::vector<T>& vec) noexcept {
        for (auto& obj : vec) {
            objects.push_back(&obj);
        }
    }


    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;

    aabb bounding_box() const override {
        return bbox;
    }

private:
    aabb bbox;
};

template<hittable_concept... hittables>
class bvh_node : public hittable {
public:
    // Calls list.build() automatically before constructing.
    bvh_node(hittable_list<hittables...> list, std::vector<std::unique_ptr<bvh_node>>& node_list) : node_list(node_list) {
        list.build();
        init(list.objects, 0, list.objects.size());
    }
    bvh_node(std::vector<std::variant<hittables*...>>& objects, size_t start, size_t end, std::vector<std::unique_ptr<bvh_node>>& node_list) : node_list(node_list) {
        init(objects, start, end);
    }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
    aabb bounding_box() const override { return bbox; }

private:
    void init(std::vector<std::variant<hittables*...>> &objects, size_t start, size_t end);

    std::variant<std::variant<hittables*...>, bvh_node*> left = (bvh_node*)nullptr;
    std::variant<std::variant<hittables*...>, bvh_node*> right = (bvh_node*)nullptr;
    std::vector<std::unique_ptr<bvh_node>>& node_list;
    aabb bbox;

    static bool box_compare(hittable* a, hittable* b, int axis_index) {
        return a->bounding_box().axis_interval(axis_index).min < b->bounding_box().axis_interval(axis_index).min;
    }
    static bool box_x_compare(hittable* a, hittable* b) { return box_compare(a, b, 0); }
    static bool box_y_compare(hittable* a, hittable* b) { return box_compare(a, b, 1); }
    static bool box_z_compare(hittable* a, hittable* b) { return box_compare(a, b, 2); }
};


template<hittable_concept... hittables>
tiny::optional<hit_record> hittable_list<hittables...>::hit(const ray &r, interval ray_t) const noexcept {
    return std::nullopt;
    /*
    if (!bbox.hit(r, ray_t)) return std::nullopt;
    tiny::optional<hit_record> rec = std::nullopt;
    float closest = ray_t.max;

    for (auto* obj : objects) {
        auto temp = obj->hit(r, interval(ray_t.min, closest));
        if (temp) {
            closest = temp->t;
            rec = temp;
        }
    }

    return rec;
    */
}


template<hittable_concept... hittables>
constexpr std::variant<hittables*..., bvh_node<hittables...>*> variant_convert(const std::variant<std::variant<hittables*...>, bvh_node<hittables...>*>* variant) noexcept {
    auto bvh_ptr = std::get_if<bvh_node<hittables...>*>(variant);
    if (bvh_ptr) {
        return *bvh_ptr;
    }
    auto obj = std::get_if<std::variant<hittables*...>>(variant);
    auto sphere_ptr = std::get_if<sphere*>(obj);
    if (sphere_ptr) {
        return *sphere_ptr;
    }
    auto tri_ptr = std::get_if<tri*>(obj);
    if (tri_ptr) {
        return *tri_ptr;
    }
    auto quad_ptr = std::get_if<quad*>(obj);
    if (quad_ptr) {
        return *quad_ptr;
    }
    return (bvh_node<hittables...>*)nullptr;
}

template<hittable_concept... hittables>
void bvh_node<hittables...>::init(std::vector<std::variant<hittables*...>>& objects, size_t start, size_t end) {
    int axis = random_int(0, 2);

    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = objects[start];
        right = (bvh_node*)nullptr;
    } else if (object_span == 2) {
        left = objects[start];
        right = objects[start + 1];
    } else {
//        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        auto mid = start + object_span / 2;
        left = node_list.emplace_back(std::make_unique<bvh_node>(objects, start, mid, node_list)).get();
        right = node_list.emplace_back(std::make_unique<bvh_node>(objects, mid, end, node_list)).get();
    }

    auto tmp_left = variant_convert(&left);
    auto tmp_right = variant_convert(&right);

    if (std::visit([](auto&& arg){ return arg != nullptr; }, tmp_right)) {
        bbox = aabb(std::visit([](auto&& arg){ return arg->bounding_box(); }, tmp_left), std::visit([](auto&& arg){ return arg->bounding_box(); }, tmp_right));
    } else {
        bbox = std::visit([](auto&& arg){ return arg->bounding_box(); }, tmp_left);
    }
}

template<hittable_concept... hittables>
tiny::optional<hit_record> bvh_node<hittables...>::hit(const ray& r, interval ray_t) const noexcept {
    if (!bbox.hit(r, ray_t)) 
        return std::nullopt;

    auto tmp_left = variant_convert(&left);
    auto tmp_right = variant_convert(&right);

    auto hit_left = std::visit([&](auto&& arg) { return arg->hit(r, ray_t); }, tmp_left);
    if (std::visit([](auto&& arg){ return arg == nullptr; }, tmp_right)) { return hit_left; }

    auto hit_right = std::visit([&](auto&& arg) { return arg->hit(r, interval(ray_t.min, hit_left ? hit_left->t : ray_t.max)); }, tmp_right);
    return hit_right ? hit_right : hit_left;
}
