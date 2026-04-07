#include "hittable.h"
#include "sphere.h"
#include "tri.h"

class hittable_list : public hittable {
public:
    std::vector<sphere> spheres;
    std::vector<tri>    tris;
    std::vector<quad>   quads;

    hittable_list() noexcept {}

    void clear() { spheres.clear(); tris.clear(); quads.clear(); }

    void add(sphere s) noexcept { spheres.push_back(std::move(s)); }
    void add(tri t)    noexcept { tris.push_back(std::move(t)); }
    void add(quad q)   noexcept { quads.push_back(std::move(q)); }

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
};
