#include "hittable.h"
#include "vec3.h"
#include <cstring>
#include "camera.h"

void camera::render(const hittable& world) {

    initialize();

    decltype(this) camera_ptr = this;

    for (int i = 0; i < thread_count; i++) {
        render_threads[i] = std::thread(&camera::thread_render, camera_ptr, std::cref(world), i);
    }


    for (auto& thread : render_threads) {
        thread.join();
    }

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            write_color(std::cout, result[j * image_width + i]);
        }
    }


}

void camera::initialize() {

    image_height = calculate_height(image_width, aspect_ratio);
    
    if ((image_height * image_width) % thread_count != 0) {
        std::cerr << "Error: image dimensions must be divisible by thread count" << std::endl;
        exit(-1);
    }

    viewport_height = calculate_viewport_height();
    viewport_width = viewport_height * (float(image_width) / image_height);
    camera_center = lookfrom;
    pixel_samples_scale = 1.0 / samples_per_pixel;
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    vec3 viewport_u = viewport_width * u;
    vec3 viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    point3 viewport_upper_left = camera_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    float defocus_radius = std::tan(degrees_to_radians(defocus_angle)) * focus_dist;
    defocus_disk_u = defocus_radius * u;
    defocus_disk_v = defocus_radius * v;

    result.resize(image_width * image_height);
}

void camera::thread_render(const hittable& world, const int thread_num) {
    ssize_t start_pixel = thread_num * result.size() / thread_count;
    ssize_t pixel_count = result.size() / this->thread_count;
    color3* buffer = new color3[pixel_count];
    for (int p = 0; p < pixel_count; p++) {
        int i = ((p + start_pixel) % image_width);
        int j = ((p + start_pixel) / image_width);
        color3 pixel_color(0,0,0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
            ray r = get_ray(i, j);
            pixel_color += ray_color(r, world, 0);
        }
        buffer[p] = pixel_samples_scale * pixel_color;
    }
    memcpy(&result[start_pixel], buffer, pixel_count * sizeof(color3));


}

color3 camera::ray_color(const ray& r, const hittable& world, int depth) const {
    if (depth > max_depth) {
        return color3(0,0,0);
    }
    auto rec = world.hit(r, interval(0.001, infinity));
    if (!rec) {
        return background_color;
    }
    auto scatter_result = rec->mat_ptr->scatter(r, *rec);
    color3 emitted = rec->mat_ptr->emitted();
    if (!scatter_result) {
        return emitted;
    }
    return scatter_result->first * ray_color(scatter_result->second, world, depth + 1) + emitted;

}

ray camera::get_ray(int i, int j) const {

    vec3 offset = sample_square();
    point3 pixel_sample = pixel00_loc
        + ((i + offset.x()) * pixel_delta_u)
        + ((j + offset.y()) * pixel_delta_v);

    point3 ray_origin;
    if (defocus_angle <= 0) {
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

