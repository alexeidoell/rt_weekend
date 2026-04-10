#include "color3.h"
#include "common.h"
#include "hittable.h"
#include "vec3.h"
#include <cstdint>
#include <cstring>
#include "camera.h"
#include <fstream>
#include <memory>
#include <new>
#include <print>
#include <spng.h>
#include <barkeep/barkeep.h>

int writeout(spng_ctx* ctx, void* user, void* data, size_t length) {
    static std::ofstream strm("test.png");
    strm.write((const char*)data, length);
    return 0;
}

spng_ctx* init_spng(uint32_t width, uint32_t height) {
    spng_ctx* ctx = spng_ctx_new(SPNG_CTX_ENCODER);
    spng_set_png_stream(ctx, writeout, nullptr);
    
    struct spng_ihdr ihdr = {0};

    ihdr.width = width;
    ihdr.height = height;
    ihdr.color_type = SPNG_COLOR_TYPE_TRUECOLOR;
    ihdr.bit_depth = 8;

    spng_set_ihdr(ctx, &ihdr);

    return ctx;

}

void camera::render(const hittable& world) {

    initialize();

    spng_ctx* ctx = init_spng(image_width, image_height);

    decltype(this) camera_ptr = this;

    for (int i = 0; i < thread_count - 1; i++) {
        render_threads[i] = std::thread(&camera::thread_render<false>, camera_ptr, std::cref(world));
    }

    auto bar = barkeep::ProgressBar([&] { return image_height * image_width / pixel_count - render_tasks.size(); }, {
            .total = image_width * image_height / pixel_count,
            .speed = 1.,
            .speed_unit = "chunks/s",
            .style = barkeep::ProgressBarStyle::Rich,
            .interval = std::chrono::milliseconds(100),
            });
    bar->show();
    thread_render<true>(world);
     

    for (auto& thread : render_threads) {
        thread.join();
    }


    if(spng_encode_image(ctx, result.get(), image_width * image_height * 3, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE)) {
        std::cerr << "Failed to encode PNG image" << std::endl;
    }

}

void camera::initialize() {

    image_height = calculate_height(image_width, aspect_ratio);
    
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

    result = std::unique_ptr<uint8_t[]>(new (std::align_val_t(64)) uint8_t[image_height * image_width * 3]);
//    result = std::make_unique<uint8_t[]>(image_height * image_width * 3);


    for (int i = 0; i < image_height * image_width; i += pixel_count) {
        render_tasks.push(i);
    }

}

template <bool owner>
void camera::thread_render(const hittable& world) {
    static hn::ScalableTag<float> d;
    static hn::ScalableTag<uint32_t> d_int;
    static hn::CappedTag<uint8_t, d_int.MaxLanes()> d_byte;
    static hn::CappedTag<float, 4> d_vec;
    static auto const_intensity = hn::Set(d, 255.0f);
    static auto const_clamp_lo = hn::Set(d, 0.0f);
    static auto const_clamp_hi = hn::Set(d, 0.999f);
    static auto const_scale = hn::Set(d, pixel_samples_scale);
    auto float_buffer = std::unique_ptr<float[]>(new (std::align_val_t(64)) float[pixel_count * 3]);
    while (!render_tasks.empty()) {
        std::optional<int> task;
        if constexpr (owner) {
            task = render_tasks.pop();
//            std::print("{}% complete\n", 100 - (render_tasks.size() * pixel_count * 100 / (image_width * image_height)));
        } else {
            task = render_tasks.steal();
        }
        if (!task) {
            continue;
        }
        ssize_t start_pixel = *task;
        for (int p = 0; p < pixel_count; p++) {
            int i = ((p + start_pixel) % image_width);
            int j = ((p + start_pixel) / image_width);
            color3 pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; sample++) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, world, 0);
            }
            int byte_index = p * 3;
            hn::StoreN(pixel_color.vec, d_vec, &float_buffer[byte_index], 3);
        }
        for (int i = 0; i < pixel_count * 3; i += d.MaxLanes()) {
            auto vec = hn::Load(d, float_buffer.get() + i);
            auto mask = hn::IsNegative(vec);
            vec = hn::Mul(hn::IfThenZeroElse(mask, vec), const_scale);
            vec = hn::Sqrt(vec);
            vec = hn::Clamp(vec, const_clamp_lo, const_clamp_hi);
            vec = hn::Mul(vec, const_intensity);
            auto byte_vec = hn::U8FromU32(hn::ConvertTo(d_int, vec));
            hn::Store(byte_vec, d_byte, &result[start_pixel * 3 + i]);
        }
//        memcpy(&result[start_pixel * 3], int_buffer.get(), pixel_count * 3);
    }
}

color3 camera::ray_color(const ray& r, const hittable& world, int depth) const {
    if (depth > max_depth) {
        return color3(0,0,0);
    }
    auto rec = world.hit(r, interval(0.001, infinity));
    if (!rec) {
        return background_color;
    }
    const material * const mat_ptr = rec->mat_ptr;
    auto scatter_result = mat_ptr->scatter(r, *rec);
    color3 emitted = mat_ptr->emitted();
    if (!scatter_result) {
        return emitted;
    }
    return color3(hn::MulAdd(mat_ptr->get_albedo().vec, ray_color(*scatter_result, world, depth + 1).vec, emitted.vec));

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

    float ray_time = random_double();

    return ray(ray_origin, ray_direction, ray_time);
}

vec3 camera::sample_square() const {
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

vec3 camera::defocus_disk_sample() const {
    vec3 p = random_in_unit_disk();
    return camera_center + p.x() * defocus_disk_u + p.y() * defocus_disk_v;
}

