#pragma once


#include "color3.h"
#include "hittable.h"
class camera {
public:
    static constexpr double aspect_ratio = 16.0 / 9.0;
    static constexpr int image_width = 3840;

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                vec3 ray_direction = pixel_center - camera_center;
                ray r(camera_center, ray_direction);

                write_color(std::cout, ray_color(r, world));
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

    color3 ray_color(const ray& r, const hittable& world) const {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec)) {
            return 0.5 * (rec.normal + color3(1, 1, 1));
        }

        vec3 unit_vec = unit_vector(r.direction());
        double a = 0.5 * (unit_vec.y() + 1.0);
        return (1.0 - a) * color3(1.0, 1.0, 1.0) + a * color3(0.5, 0.7, 1.0);
    }

};
