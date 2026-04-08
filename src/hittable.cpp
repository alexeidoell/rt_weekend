#include "hittable.h"
#include "hittable_list.h"
#include <tiny/optional.h>
#include <variant>

void hit_record::set_face_normal(const ray& r, const vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

tiny::optional<hit_record> hittable_list::hit(const ray &r, interval ray_t) const noexcept {
    if (!bbox.hit(r, ray_t)) return std::nullopt;
    tiny::optional<hit_record> rec = std::nullopt;
    float closest = ray_t.max;

    for (const auto &s : objects) {
        auto temp = std::visit(
                [&](auto &&obj) { return obj.hit(r, interval(ray_t.min, closest)); },
                s);

        if (temp) {
            closest = temp->t;
            rec = temp;
        }
    }

    return rec;
}
