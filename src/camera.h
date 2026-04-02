#pragma once

#include <thread>
#include <array>
#include <vector>
#include "color3.h"
#include "hittable.h"
#include "vec3.h"
#include "material.h"

inline constexpr float calculate_height(float image_width, float aspect_ratio) {
    float image_height = static_cast<int>(image_width / aspect_ratio);
    return image_height < 1 ? 1 : image_height;
}

class camera {
public:
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 1200;
    int samples_per_pixel = 5;
    int max_depth = 50;
    int vfov = 20;
    float defocus_angle = 0.6;
    float focus_dist = 10.0;
    color3 background_color = color3(0,0,0);

    point3 lookfrom = point3(13,2,3);
    point3 lookat = point3(0,0,0);
    vec3 vup = vec3(0,1,0);


    void render(const hittable& world);


private:
    constexpr float calculate_viewport_height() {
        float theta = degrees_to_radians(vfov);
        float h = std::tan(theta/2);
        return 2 * h * focus_dist;

    }

    int image_height;
    float viewport_height;
    float viewport_width;
    point3 camera_center;
    float pixel_samples_scale;

    vec3 pixel_delta_u, pixel_delta_v;
    point3 pixel00_loc;
    vec3 u, v, w;

    vec3 defocus_disk_u, defocus_disk_v;

    static const int thread_count = 12;
    std::array<std::thread, thread_count> render_threads;
    std::vector<color3> result;

    void initialize();
    void thread_render(const hittable& world, const int thread_num);
    color3 ray_color(const ray& r, const hittable& world, int depth) const;
    ray get_ray(int i, int j) const;
    vec3 sample_square() const;
    vec3 defocus_disk_sample() const;

};

