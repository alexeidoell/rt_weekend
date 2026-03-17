#pragma once


#include "../tiny-optional/include/tiny/optional.h"
#include <utility>
#include "common.h"
#include "color3.h"
#include "hittable.h"
#include "vec3.h"

class material {
public:
    virtual ~material() = default;

    virtual tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const {
        return std::nullopt;
    }
};

class lambertian : public material {
public:
    lambertian(const color3& albedo) : albedo(albedo) {}
    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const override {
        double random = random_double(0, 1);
        if (random < -0.3) {
            return std::nullopt;
        }
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        return tiny::make_optional<std::pair<color3,ray>>(albedo, ray(rec.p, scatter_direction));
    }


private:
    color3 albedo;


};

class metal : public material {
  public:
    metal(const color3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + fuzz * random_unit_vector();
        if (dot(reflected, rec.normal) > 0) {
            return tiny::make_optional<std::pair<color3,ray>>(albedo, ray(rec.p, reflected));
        }
        return std::nullopt;
    }

  private:
    color3 albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec)
    const override {
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
        vec3 direction;

        if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_double()) { // cannot refract
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, ri);

        }
        return tiny::make_optional<std::pair<color3,ray>>(color3(1.0,1.0,1.0), ray(rec.p, direction));
    }

private:
    double refraction_index;

    constexpr static double reflectance(double cosine, double ri) {
        auto r0 = (1-ri) / (1+ri);
        r0 = r0*r0;
        return r0 + (1-r0) * std::pow((1 - cosine), 5);
    }
};
