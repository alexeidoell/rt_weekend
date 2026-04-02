#include "vec3.h"
#include "vectorization.h"
#include <cassert>
#include <cmath>

vec3::vec3() : e{0, 0, 0} {
    vec = hwy::N_SSE2::Set(hwy::N_SSE2::CappedTag<float, 4>(), 0);
}
vec3::vec3(float e0, float e1, float e2) : e{e0, e1, e2} {
    vec = hwy::N_SSE2::LoadN(hwy::N_SSE2::CappedTag<float, 4>(), e, 3);
}

vec3::vec3(hwy::N_SSE2::Vec128<float> vec) : vec(vec) {
    hwy::N_SSE2::StoreN(vec, hwy::N_SSE2::CappedTag<float, 4>(), e, 3);
}

float vec3::x() const { return e[0]; }
float vec3::y() const { return e[1]; }
float vec3::z() const { return e[2]; }

vec3 vec3::operator-() const { return vec3(-e[0], -e[1], -e[2]); }
float& vec3::operator[](std::size_t i) { return e[i]; }
const float& vec3::operator[](std::size_t i) const { return e[i]; }

vec3& vec3::operator+=(const vec3& v) {
    vec = hwy::N_SSE2::Add(vec, v.vec);
    hwy::N_SSE2::StoreN(vec, hwy::N_SSE2::CappedTag<float, 4>(), e, 3);
    return *this;
}

vec3& vec3::operator*=(float t) {
    vec = hwy::N_SSE2::Mul(vec, hwy::N_SSE2::Set(hwy::N_SSE2::CappedTag<float, 4>(), t));
    hwy::N_SSE2::StoreN(vec, hwy::N_SSE2::CappedTag<float, 4>(), e, 3);
    return *this;
}

vec3& vec3::operator/=(float t) {
    return *this *= 1/t;
}

float vec3::length() const {
    return std::sqrt(length_squared());
}

 float vec3::length_squared() const {
    return N_SSE2::dot_packed(vec, vec);
}

bool vec3::near_zero() const {
    auto s = 1e-8;
    const hwy::N_SSE2::CappedTag<float, 4> d;
    auto comp = hwy::N_SSE2::Set(d, s);
    auto result = hwy::N_SSE2::Lt(vec, comp);
    return hwy::N_SSE2::AllTrue(d, result);
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
    return vec3(hwy::N_SSE2::Add(u.vec, v.vec));

}

vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(hwy::N_SSE2::Sub(u.vec, v.vec));
}

vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(hwy::N_SSE2::Mul(u.vec, v.vec));
}

vec3 operator*(float t, const vec3& v) {
    return vec3(hwy::N_SSE2::Mul(hwy::N_SSE2::Set(hwy::N_SSE2::CappedTag<float, 4>(), t), v.vec));
}

vec3 operator*(const vec3& v, float t) {
    return t * v;
}

vec3 operator/(const vec3& v, float t) {
    return (1/t) * v;
}

bool operator==(const vec3& u, const vec3& v) {
    return hwy::N_SSE2::AllTrue(hwy::N_SSE2::CappedTag<float, 4>(), hwy::N_SSE2::Eq(u.vec, v.vec));
}

float dot(const vec3& u, const vec3& v) {
    return N_SSE2::dot_packed(u.vec, v.vec);
}

vec3 cross(const vec3& u, const vec3& v) {
    const hwy::N_SSE2::CappedTag<float, 4> d;
    auto u_shuffled = hwy::N_SSE2::Per4LaneBlockShuffle<1, 0, 2, 1>(u.vec);
    auto v_shuffled = hwy::N_SSE2::Per4LaneBlockShuffle<1, 0, 2, 1>(v.vec);

    auto right = Mul(hwy::N_SSE2::SlideDownLanes(d, u_shuffled, 1), v_shuffled);
    vec3 result = vec3(MulSub(u_shuffled, hwy::N_SSE2::SlideDownLanes(d, v_shuffled, 1), right));
    
    return result;
}

vec3 unit_vector(const vec3& v) {
    return v / v.length();
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
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
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

