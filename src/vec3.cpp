#include "vec3.h"
#include <immintrin.h>
#include <cassert>
#include <cmath>

vec3::vec3() : e{0, 0, 0} {}

vec3::vec3(float e0, float e1, float e2) : e{e0, e1, e2} {}

float vec3::x() const { return e[0]; }
float vec3::y() const { return e[1]; }
float vec3::z() const { return e[2]; }

vec3 vec3::operator-() const { return vec3(-e[0], -e[1], -e[2]); }
float& vec3::operator[](std::size_t i) { return e[i]; }
const float& vec3::operator[](std::size_t i) const { return e[i]; }

vec3& vec3::operator+=(const vec3& v) {
    e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2];
    return *this;
}

vec3& vec3::operator*=(float t) {
    e[0] *= t; e[1] *= t; e[2] *= t;
    return *this;
}

vec3& vec3::operator/=(float t) {
    return *this *= 1/t;
}

float vec3::length() const {
    return std::sqrt(length_squared());
}

float vec3::length_squared() const {
    return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
}

bool vec3::near_zero() const {
    auto s = 1e-8f;
    return std::fabs(e[0]) < s && std::fabs(e[1]) < s && std::fabs(e[2]) < s;
}

vec3 vec3::random() {
    return vec3(random_double(), random_double(), random_double());
}

vec3 vec3::random(float min, float max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0]+v.e[0], u.e[1]+v.e[1], u.e[2]+v.e[2]);
}

vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0]-v.e[0], u.e[1]-v.e[1], u.e[2]-v.e[2]);
}

vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0]*v.e[0], u.e[1]*v.e[1], u.e[2]*v.e[2]);
}

vec3 operator*(float t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

vec3 operator*(const vec3& v, float t) {
    return t * v;
}

vec3 operator/(const vec3& v, float t) {
    return (1/t) * v;
}

bool operator==(const vec3& u, const vec3& v) {
    return u.e[0]==v.e[0] && u.e[1]==v.e[1] && u.e[2]==v.e[2];
}

float dot(const vec3& u, const vec3& v) {
    return u.e[0]*v.e[0] + u.e[1]*v.e[1] + u.e[2]*v.e[2];
}

vec3 cross(const vec3& u, const vec3& v) {
    return vec3(
        u.e[1]*v.e[2] - u.e[2]*v.e[1],
        u.e[2]*v.e[0] - u.e[0]*v.e[2],
        u.e[0]*v.e[1] - u.e[1]*v.e[0]
    );
}

vec3 unit_vector(const vec3& v) {
    return v * _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(v.length_squared())));
}

vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

// marsaglia formula
// only needs 2 random numbers
vec3 random_unit_vector() {
    while (true) {
        float x1 = random_double(-1, 1);
        float x2 = random_double(-1, 1);
        float test = x1*x1 + x2*x2;
        if (1e-160 < test && test <= 1) {
            float sqrt_term = std::sqrt(1 - test);
            return vec3(2*x1*sqrt_term, 2*x2*sqrt_term, 1 - 2*test);
        }
    }
}

vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

vec3 refract(const vec3& uv, const vec3& n, float etai_over_etap) {
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etap * (uv + cos_theta*n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

