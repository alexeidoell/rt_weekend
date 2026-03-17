#include "hittable.h"
#include "camera.h"

void camera::render(const hittable& world) {

    initialize();

    decltype(this) camera_ptr = this;

    for (int i = 0; i < thread_count; i++) {
        render_threads[i] = std::thread(&camera::thread_render, camera_ptr, std::ref(thread_buffers[i]), std::cref(world));
    }

    for (auto& thread : render_threads) {
        thread.join();
    }

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            result[j * image_width + i] = color3(0,0,0);
            for (int t = 0; t < thread_count; t++) {
                result[j * image_width + i] += thread_buffers[t][j * image_width + i];
            }
            result[j * image_width + i] *= pixel_samples_scale;
            write_color(std::cout, result[j * image_width + i]);
        }
    }


}



void camera::initialize() {
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

    result.reserve(image_width * image_height);
}

void camera::thread_render(std::vector<color3>& buffer, const hittable& world) {
    buffer.reserve(image_width * image_height);
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

color3 camera::ray_color(const ray& r, const hittable& world, int depth) const {
    if (depth > max_depth) {
        return color3(0,0,0);
    }
    auto rec = world.hit(r, interval(0.001, infinity));
    if (rec) {
        auto scatter_result = rec->mat_ptr->scatter(r, *rec);
        if (scatter_result) {
            return scatter_result->first * ray_color(scatter_result->second, world, depth + 1);
        }
        return color3(0,0,0);
    }

    vec3 unit_vec = unit_vector(r.direction());
    double a = 0.5 * (unit_vec.y() + 1.0);
    return (1.0 - a) * color3(1.0, 1.0, 1.0) + a * color3(0.5, 0.7, 1.0);
}

ray camera::get_ray(int i, int j) const {

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

vec3 camera::sample_square() const {
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

vec3 camera::defocus_disk_sample() const {
    vec3 p = random_in_unit_disk();
    return camera_center + p.x() * defocus_disk_u + p.y() * defocus_disk_v;
}

