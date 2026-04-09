#pragma once

#include "aabb.h"
#include "hittable.h"
#include "sphere.h"
#include "tri.h"
#include <vector>
#include <algorithm>
#include <tiny/optional.h>

class hittable_list : public hittable {
public:
    std::vector<sphere> spheres;
    std::vector<tri> tris;
    std::vector<quad> quads;
    std::vector<hittable*> objects; // raw ptrs into above; populated by build()

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

    // Call after all adds. Populates objects[] and recomputes bbox.
    void build() noexcept {
        objects.clear();
        for (auto& s : spheres) { objects.push_back(&s);  }
        for (auto& t : tris)    { objects.push_back(&t);  }
        for (auto& q : quads)   { objects.push_back(&q);  }
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
    bvh_node(std::vector<hittable*>& objects, size_t start, size_t end, std::vector<std::unique_ptr<bvh_node>>& node_list) : node_list(node_list) {
        init(objects, start, end);
    }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
    aabb bounding_box() const override { return bbox; }

private:
    void init(std::vector<hittable*>& objects, size_t start, size_t end);

    hittable* left = nullptr;
    hittable* right = nullptr;
    std::vector<std::unique_ptr<bvh_node>>& node_list;
    aabb bbox;

    static bool box_compare(hittable* a, hittable* b, int axis_index) {
        return a->bounding_box().axis_interval(axis_index).min < b->bounding_box().axis_interval(axis_index).min;
    }
    static bool box_x_compare(hittable* a, hittable* b) { return box_compare(a, b, 0); }
    static bool box_y_compare(hittable* a, hittable* b) { return box_compare(a, b, 1); }
    static bool box_z_compare(hittable* a, hittable* b) { return box_compare(a, b, 2); }
};
