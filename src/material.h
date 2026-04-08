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

    inline virtual color3 get_albedo() const {
        return color3(0,0,0);
    };

    virtual color3 emitted() const {
        return color3(0,0,0);
    }

    virtual tiny::optional<ray> scatter(const ray& r_in, const hit_record& rec) const {
        return std::nullopt;
    }
};

class lambertian : public material {
public:
    lambertian(const color3& albedo) : albedo(albedo) {}
    tiny::optional<ray> scatter(const ray& r_in, const hit_record& rec) const override;
    inline color3 get_albedo() const override {
        return albedo;
    }
private:
    color3 albedo;


};

class metal : public material {
  public:
    metal(const color3& albedo, float fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    tiny::optional<ray> scatter(const ray& r_in, const hit_record& rec) const override;
    inline color3 get_albedo() const override {
        return albedo;
    }
  private:
    color3 albedo;
    float fuzz;
};

class dielectric : public material {
public:
    dielectric(float refraction_index) : refraction_index(refraction_index) {}

    tiny::optional<ray> scatter(const ray& r_in, const hit_record& rec) const override;
    inline color3 get_albedo() const override {
        return color3(1,1,1);
    }
private:
    float refraction_index;

    constexpr static float reflectance(float cosine, float ri) {
        auto r0 = (1-ri) / (1+ri);
        r0 = r0*r0;
        return r0 + (1-r0) * std::pow((1 - cosine), 5);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(const color3& emitted_color) : emitted_color(emitted_color) {}
    tiny::optional<ray> scatter(const ray& r_in, const hit_record& rec) const override;
    color3 emitted() const override;
private:
    color3 emitted_color;
};
