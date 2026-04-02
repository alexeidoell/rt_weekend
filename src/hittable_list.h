#include "hittable.h"
#include "sphere.h"
#include "tri.h"

class hittable_list : public hittable {
public:
    using variant_type = std::variant<const sphere, const tri, const quad>;
    std::vector<variant_type> objects;

    hittable_list() noexcept {}

    void clear(); 

    void add(const variant_type &&object) noexcept;

    tiny::optional<hit_record> hit(const ray& r, interval ray_t) const noexcept override;
};
