#include "material.h"

tiny::optional<ray> lambertian::scatter(const ray& r_in, const hit_record& rec) const {
    float random = random_double(0, 1);
    if (random < -0.3) {
        return std::nullopt;
    }
    auto scatter_direction = rec.normal + random_unit_vector();

    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    return tiny::make_optional<ray>(rec.p, scatter_direction, r_in.time());
}

tiny::optional<ray> metal::scatter(const ray& r_in, const hit_record& rec) const {
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = unit_vector(reflected) + fuzz * random_unit_vector();
    if (dot(reflected, rec.normal) > 0) {
        return tiny::make_optional<ray>(rec.p, reflected, r_in.time());
    }
    return std::nullopt;
}

tiny::optional<ray> dielectric::scatter(const ray& r_in, const hit_record& rec) const {
    float ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

    vec3 unit_direction = unit_vector(r_in.direction());
    float cos_theta = std::fminf(dot(-unit_direction, rec.normal), 1.0);
    float sin_theta = std::sqrtf(1.0 - cos_theta*cos_theta);
    vec3 direction;

    if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_double()) { // cannot refract
        direction = reflect(unit_direction, rec.normal);
    } else {
        direction = refract(unit_direction, rec.normal, ri);

    }
    return tiny::make_optional<ray>(rec.p, direction, r_in.time());
}

tiny::optional<ray> diffuse_light::scatter(const ray& r_in, const hit_record& rec) const {
    return std::nullopt;
}

color3 diffuse_light::emitted() const {
    return emitted_color;
}
