#include "hittable.h"
#include "hittable_list.h"
#include <tiny/optional.h>
#include <variant>

void hit_record::set_face_normal(const ray& r, const vec3& outward_normal) {
    // outward_normal must be a unit vector
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

void hittable_list::clear() { 
    objects.clear(); 
}

void hittable_list::add(const variant_type &&object) noexcept {
    objects.push_back(std::move(object));
}

tiny::optional<hit_record> hittable_list::hit(const ray& r, interval ray_t) const noexcept {
    tiny::optional<hit_record> rec = std::nullopt;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
        tiny::optional<hit_record> temp_rec = std::visit([&](auto&& obj) { return obj.hit(r, interval(ray_t.min, closest_so_far)); }, object);
        if (temp_rec) {
            closest_so_far = temp_rec->t;
            rec = temp_rec;
        }
    }

    return rec;
}
