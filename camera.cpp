#include "hittable.h"
#include "camera.h"

void camera::render(const hittable& world) {

    initialize();

    decltype(this) camera_ptr = this;

    for (int i = 0; i < threads; i++) {
        render_threads[i] = std::thread(&camera::thread_render, camera_ptr, std::ref(thread_buffers[i]), std::cref(world));
    }

    for (auto& thread : render_threads) {
        thread.join();
    }

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            result[j * image_width + i] = color3(0,0,0);
            for (int t = 0; t < threads; t++) {
                result[j * image_width + i] += thread_buffers[t][j * image_width + i];
            }
            result[j * image_width + i] *= pixel_samples_scale;
            write_color(std::cout, result[j * image_width + i]);
        }
    }


}


