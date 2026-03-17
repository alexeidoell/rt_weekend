#pragma once

#include <thread>
#include <array>
#include <vector>
#include "color3.h"
#include "hittable.h"
#include "vec3.h"
#include "material.h"

inline constexpr double calculate_height(double image_width, double aspect_ratio) {
    double image_height = static_cast<int>(image_width / aspect_ratio);
    return image_height < 1 ? 1 : image_height;
}

class camera {
public:
    static constexpr double aspect_ratio = 16.0 / 9.0;
    static constexpr int image_width = 1200;
    static constexpr int samples_per_pixel = 1;
    static constexpr int max_depth = 50;
    static constexpr int vfov = 20;
    static constexpr double defocus_angle = 0.6;
    static constexpr double focus_dist = 10.0;

    point3 lookfrom = point3(13,2,3);
    point3 lookat = point3(0,0,0);
    vec3 vup = vec3(0,1,0);


    void render(const hittable& world);


private:
    static constexpr double calculate_viewport_height() {
        double theta = degrees_to_radians(vfov);
        double h = std::tan(theta/2);
        return 2 * h * focus_dist;

    }

    static constexpr int image_height = calculate_height(image_width, aspect_ratio);
    double viewport_height = calculate_viewport_height();
    double viewport_width = viewport_height * (double(image_width) / image_height);
    point3 camera_center = lookfrom;
    double pixel_samples_scale = 1.0 / samples_per_pixel;

    vec3 pixel_delta_u, pixel_delta_v;
    point3 pixel00_loc;
    vec3 u, v, w;

    vec3 defocus_disk_u, defocus_disk_v;

    static const int thread_count = 12;
    std::array<std::thread, thread_count> render_threads;
    std::array<std::vector<color3>, thread_count> thread_buffers;
    std::vector<color3> result;

    void initialize();
    void thread_render(std::vector<color3>& buffer, const hittable& world);
    color3 ray_color(const ray& r, const hittable& world, int depth) const;
    ray get_ray(int i, int j) const;
    vec3 sample_square() const;
    vec3 defocus_disk_sample() const;

};

