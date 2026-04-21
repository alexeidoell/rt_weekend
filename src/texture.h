#pragma once
#include "color3.h"
#include "perlin.h"
#include "vec3.h"
#include <memory>

class texture {
  public:
    virtual ~texture() = default;

    virtual color3 value(float u, float v, const point3& p) const = 0;
};

class solid_color : public texture {
  public:
    solid_color(const color3& albedo) : albedo(albedo) {}

    solid_color(float red, float green, float blue) : solid_color(color3(red,green,blue)) {}

    color3 value(float u, float v, const point3& p) const override {
        return albedo;
    }

  private:
    color3 albedo;
};

class noise_texture : public texture {
  public:
    noise_texture() {}

    color3 value(float u, float v, const point3& p) const override {
        return color3(1,1,1) * noise.noise(p);
    }

  private:
    perlin noise; // 4kb...
};

class checker_texture : public texture {
  public:
    checker_texture(float scale, std::shared_ptr<const texture> even, std::shared_ptr<const texture> odd) : scale(1.0/scale), even(even), odd(odd) {}
    checker_texture(float scale, const color3& even, const color3& odd) : scale(1.0/scale), even(std::make_shared<solid_color>(even)), odd(std::make_shared<solid_color>(odd)) {}

    color3 value(float u, float v, const point3& p) const override {
        static constexpr auto d = vec3::get_tag();
        static constexpr auto d_int = hn::CappedTag<int, 4>{};
        auto p_even = hn::ReduceSum(d_int, hn::ConvertTo(d_int, hn::Round(hn::Mul(hn::Set(d, scale), p.vec)))) % 2 == 0;

        return p_even ? even->value(u, v, p) : odd->value(u, v, p);
    }

  private:
    std::shared_ptr<const texture> even;
    std::shared_ptr<const texture> odd;
    float scale;
};
