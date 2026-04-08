#include "vec3.h"
#include "vectorization.h"
#include <immintrin.h>
#include <cmath>

vec3::vec3() {
    vec = hn::Set(hn::CappedTag<float, 4>(), 0.f);
}

vec3::vec3(float e0, float e1, float e2) {
    float arr[4] = {e0, e1, e2, 0.f};
    vec = hn::LoadN(hn::CappedTag<float, 4>(), arr, 3);
}

vec3::vec3(hn::Vec128<float> vec) : vec(vec) {
}

float vec3::x() const { return vec.raw[0]; }
float vec3::y() const { return vec.raw[1]; }
float vec3::z() const { return vec.raw[2]; }

vec3 vec3::operator-() const { return vec3(hn::Neg(vec)); }
float vec3::operator[](std::size_t i) { return vec.raw[i]; }
const float vec3::operator[](std::size_t i) const { return vec.raw[i]; }

vec3& vec3::operator+=(const vec3& v) {
    vec = hn::Add(vec, v.vec);
    return *this;
}

vec3& vec3::operator*=(float t) {
    vec = hn::Mul(vec, hn::Set(hn::CappedTag<float, 4>(), t));
    return *this;
}

vec3& vec3::operator/=(float t) {
    return *this *= 1.f/t;
}

float vec3::length() const {
    return std::sqrt(length_squared());
}

float vec3::length_squared() const {
    return dot_packed(vec, vec);
}

bool vec3::near_zero() const {
    const float s = 1e-8f;
    const hn::CappedTag<float, 4> d;
    auto comp = hn::Set(d, s);
    auto result = hn::Lt(vec, comp);
    return hn::AllTrue(d, result);
}

vec3 vec3::random() {
    return vec3(random_double(), random_double(), random_double());
}

vec3 vec3::random(float min, float max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(hn::Add(u.vec, v.vec));
}

vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(hn::Sub(u.vec, v.vec));
}

vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(hn::Mul(u.vec, v.vec));
}

vec3 operator*(float t, const vec3& v) {
    return vec3(hn::Mul(hn::Set(hn::CappedTag<float, 4>(), t), v.vec));
}

vec3 operator*(const vec3& v, float t) {
    return t * v;
}

vec3 operator/(const vec3& v, float t) {
    return (1.f/t) * v;
}

bool operator==(const vec3& u, const vec3& v) {
    return hn::AllTrue(hn::CappedTag<float, 4>(), hn::Eq(u.vec, v.vec));
}

float dot(const vec3& u, const vec3& v) {
    return dot_packed(u.vec, v.vec);
}

hn::Vec128<float> cross_vector_only(const hn::Vec128<float> u, const hn::Vec128<float> v) {
    const hn::CappedTag<float, 4> d;
    auto u_shuffled = hn::Per4LaneBlockShuffle<1, 0, 2, 1>(u);
    auto v_shuffled = hn::Per4LaneBlockShuffle<1, 0, 2, 1>(v);
    auto right = Mul(hn::SlideDownLanes(d, u_shuffled, 1), v_shuffled);
    return MulSub(u_shuffled, hn::SlideDownLanes(d, v_shuffled, 1), right);
}

vec3 cross(const vec3& u, const vec3& v) {
    return vec3(cross_vector_only(u.vec, v.vec));
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
