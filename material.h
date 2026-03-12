#pragma once


#include "common.h"
#include "color3.h"
#include "hittable.h"
#include "vec3.h"

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color3& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
public:
    lambertian(const color3& albedo) : albedo(albedo) {}
    bool scatter(const ray& r_in, const hit_record& rec, color3& attenuation, ray& scattered) const override {
        double random = random_double(0, 1);
        if (random < 0.3) {
            return false;
        }
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }


private:
    color3 albedo;


};

class metal : public material {
  public:
    metal(const color3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color3& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + fuzz * random_unit_vector();
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return dot(reflected, rec.normal) > 0;
    }

  private:
    color3 albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color3& attenuation, ray& scattered)
    const override {
        attenuation = color3(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, refracted);
        return true;
    }

private:
    double refraction_index;
};
