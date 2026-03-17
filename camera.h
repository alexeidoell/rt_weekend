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
    static constexpr int image_width = 1920;
    static constexpr int samples_per_pixel = 500;
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

    static const int threads = 8;
    std::array<std::thread, threads> render_threads;
    std::array<std::vector<color3>, threads> thread_buffers;
    std::vector<color3> result;

    void initialize() {
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        point3 viewport_upper_left = camera_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        double defocus_radius = std::tan(degrees_to_radians(defocus_angle)) * focus_dist;
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;

        result.reserve(image_height * image_width);
    }

    void thread_render(std::vector<color3>& buffer, const hittable& world) {
        buffer.reserve(image_height * image_width);
        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                color3 pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, world, 0);
                }
                buffer[j * image_width + i] = pixel_samples_scale * pixel_color;
            }
        }


    }

    color3 ray_color(const ray& r, const hittable& world, int depth) const {
        if (depth > max_depth) {
            return color3(0,0,0);
        }
        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color3 attenuation;

            if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, world, depth + 1);
            }
            return color3(0,0,0);
        }

        vec3 unit_vec = unit_vector(r.direction());
        double a = 0.5 * (unit_vec.y() + 1.0);
        return (1.0 - a) * color3(1.0, 1.0, 1.0) + a * color3(0.5, 0.7, 1.0);
    }

    ray get_ray(int i, int j) const {

        vec3 offset = sample_square();
        point3 pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        point3 ray_origin;
        if constexpr(defocus_angle <= 0) {
            ray_origin = camera_center;
        } else {
            ray_origin = defocus_disk_sample();
        }
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    vec3 defocus_disk_sample() const {
        vec3 p = random_in_unit_disk();
        return camera_center + p.x() * defocus_disk_u + p.y() * defocus_disk_v;
    }

};

