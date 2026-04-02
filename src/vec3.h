#pragma once

#include "common.h"
#include <cstddef>
#include <iostream>

class vec3 {
public:
    float e[3];
    vec3();
    vec3(float e0, float e1, float e2);


    float x() const;
    float y() const;
    float z() const;

    vec3 operator-() const;
    float& operator[](std::size_t i);
    const float& operator[](std::size_t i) const;

    vec3& operator+=(const vec3& v);
    vec3& operator*=(float t);
    vec3& operator/=(float t);

    float length() const;
    float length_squared() const;
    bool near_zero() const;

    static vec3 random();
    static vec3 random(float min, float max);
};

using point3 = vec3;

std::ostream& operator<<(std::ostream& out, const vec3& v);
vec3 operator+(const vec3& u, const vec3& v);
vec3 operator-(const vec3& u, const vec3& v);
vec3 operator*(const vec3& u, const vec3& v);
vec3 operator*(float t, const vec3& v);
vec3 operator*(const vec3& v, float t);
vec3 operator/(const vec3& v, float t);
float dot(const vec3& u, const vec3& v);
vec3 cross(const vec3& u, const vec3& v);
vec3 unit_vector(const vec3& v);
vec3 random_in_unit_disk();
vec3 random_unit_vector();
vec3 random_on_hemisphere(const vec3& normal);
vec3 reflect(const vec3& v, const vec3& n);
vec3 refract(const vec3& uv, const vec3& n, float etai_over_etap);
