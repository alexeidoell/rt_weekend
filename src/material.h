#pragma once


#include <tiny/optional.h>
#include <utility>
#include "common.h"
#include "color3.h"
#include "hittable.h"
#include "vec3.h"

class material {
public:
    virtual ~material() = default;

    virtual color3 emitted() const {
        return color3(0,0,0);
    }

    virtual tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const {
        return std::nullopt;
    }
};

class lambertian : public material {
public:
    lambertian(const color3& albedo) : albedo(albedo) {}
    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const override;
private:
    color3 albedo;


};

class metal : public material {
  public:
    metal(const color3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const override;
  private:
    color3 albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const override;
private:
    double refraction_index;

    constexpr static double reflectance(double cosine, double ri) {
        auto r0 = (1-ri) / (1+ri);
        r0 = r0*r0;
        return r0 + (1-r0) * std::pow((1 - cosine), 5);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(const color3& emitted_color) : emitted_color(emitted_color) {}
    tiny::optional<std::pair<color3, ray>> scatter(const ray& r_in, const hit_record& rec) const override;
    color3 emitted() const override;
private:
    color3 emitted_color;
};
