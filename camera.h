#pragma once


#include "color3.h"
#include "hittable.h"
#include "vec3.h"
class camera {
public:
    static constexpr double aspect_ratio = 16.0 / 9.0;
    static constexpr int image_width = 3840;
    static constexpr int samples_per_pixel = 500;
    static constexpr int max_depth = 100;

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                color3 pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, world, 0);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }


    };


private:

    constexpr double calculate_height() {
        double image_height = static_cast<int>(image_width / aspect_ratio);
        return image_height < 1 ? 1 : image_height;
    }

    double image_height = calculate_height();
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width) / image_height);
    point3 camera_center = point3(0, 0, 0);
    double pixel_samples_scale = 1.0 / samples_per_pixel;

    vec3 pixel_delta_u, pixel_delta_v;
    point3 pixel00_loc;

    void initialize() {
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        vec3 viewport_v = vec3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        point3 viewport_upper_left = camera_center + vec3(0, 0, -focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color3 ray_color(const ray& r, const hittable& world, int depth) const {
        if (depth > max_depth) {
            return color3(0,0,0);
        }
        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            vec3 direction = random_on_hemisphere(rec.normal);
            return 0.5 * ray_color(ray(rec.p, direction), world, depth + 1);
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

        point3 ray_origin = camera_center;
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

};
