#pragma once

#include "aabb.h"
#include "hittable.h"
#include "sphere.h"
#include "tri.h"
#include <print>
#include <vector>
#include <algorithm>
#include <tiny/optional.h>

class hittable_list : public hittable {
public:
    std::vector<sphere> spheres;
    std::vector<tri> tris;
    std::vector<quad> quads;
    std::vector<std::pair<hittable*, aabb>> objects; // raw ptrs into above; populated by build()

    hittable_list() noexcept {}

    void clear() { spheres.clear(); tris.clear(); quads.clear(); objects.clear(); }

    template<typename T, typename... Params>
    void add(Params... args) noexcept { 
        if constexpr (std::is_same_v<T, sphere>) {
            spheres.emplace_back(args...);
         } else if constexpr (std::is_same_v<T, quad>) {
            quads.emplace_back(args...);
         } else {
            tris.emplace_back(args...);
         }
    }

    void add(quad&& q) noexcept {
        std::println("length of quads before add: {}", quads.size());
        quads.push_back(q);
    }

    void add(tri&& t) noexcept {
        tris.push_back(std::move(t));
    }

    void add(sphere&& s) noexcept {
        spheres.push_back(std::move(s));
    }

    // Call after all adds. Populates objects[] and recomputes bbox.
    void build() noexcept {
        objects.clear();
        for (auto& s : spheres) { objects.emplace_back(&s, s.bounding_box());  }
        for (auto& t : tris)    { objects.emplace_back(&t, t.bounding_box());  }
        for (auto& q : quads)   { objects.emplace_back(&q, q.bounding_box());  }
    }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;

    aabb bounding_box() const override {
        return bbox;
    }

private:
    aabb bbox;
};

class bvh_node : public hittable {
public:
    // Calls list.build() automatically before constructing.
    bvh_node(hittable_list& list, std::vector<std::unique_ptr<bvh_node>>& node_list) : node_list(node_list) {
        list.build();
        init(list.objects, 0, list.objects.size());
    }
    bvh_node(std::vector<std::pair<hittable*, aabb>>& objects, size_t start, size_t end, std::vector<std::unique_ptr<bvh_node>>& node_list) : node_list(node_list) {
        init(objects, start, end);
    }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
    aabb bounding_box() const override { return bbox; }

private:
    void init(std::vector<std::pair<hittable*, aabb>>& objects, size_t start, size_t end);

    hittable* left = nullptr;
    hittable* right = nullptr;
    std::vector<std::unique_ptr<bvh_node>>& node_list;
    aabb bbox;

    static bool box_compare(hittable* a, hittable* b, int axis_index) {
        return a->bounding_box().axis_interval(axis_index).min < b->bounding_box().axis_interval(axis_index).min;
    }
    static bool box_x_compare(const std::pair<hittable*, aabb>& a, const std::pair<hittable*, aabb>& b) { return box_compare(a.first, b.first, 0); }
    static bool box_y_compare(const std::pair<hittable*, aabb>& a, const std::pair<hittable*, aabb>& b) { return box_compare(a.first, b.first, 1); }
    static bool box_z_compare(const std::pair<hittable*, aabb>& a, const std::pair<hittable*, aabb>& b) { return box_compare(a.first, b.first, 2); }
};
