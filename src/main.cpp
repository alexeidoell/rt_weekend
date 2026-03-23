#include <memory>
#include "camera.h"
#include "material.h"
#include "tri.h"
#include "vec3.h"
#include "color3.h"
#include "hittable.h"
#include "common.h"
#include "sphere.h"

void bouncing_spheres() {

    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(color3(0.5, 0.5, 0.5));
    world.add(std::unique_ptr<hittable>(new sphere(point3(0,-1000,0), 1000, ground_material)));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color3::random() * color3::random();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    world.add(std::unique_ptr<hittable>(new sphere(center, 0.2, sphere_material)));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::unique_ptr<hittable>(new sphere(center, 0.2, sphere_material)));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::unique_ptr<hittable>(new sphere(center, 0.2, sphere_material)));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::unique_ptr<hittable>(new sphere(point3(0, 1, 0), 1.0, material1)));

    auto material2 = std::make_shared<lambertian>(color3(0.4, 0.2, 0.1));
    world.add(std::unique_ptr<hittable>(new sphere(point3(-4, 1, 0), 1.0, material2)));

    auto material3 = std::make_shared<metal>(color3(0.7, 0.6, 0.5), 0.0);
    world.add(std::unique_ptr<hittable>(new sphere(point3(4, 1, 0), 1.0, material3)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

void quads() {
    hittable_list world;

    // Materials
    auto left_red     = std::make_shared<lambertian>(color3(1.0, 0.2, 0.2));
    auto back_green   = std::make_shared<lambertian>(color3(0.2, 1.0, 0.2));
    auto right_blue   = std::make_shared<lambertian>(color3(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<lambertian>(color3(1.0, 0.5, 0.0));
    auto lower_teal   = std::make_shared<lambertian>(color3(0.2, 0.8, 0.8));

    // Quads
    world.add(std::unique_ptr<hittable>(new quad(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red)));
    world.add(std::unique_ptr<hittable>(new quad(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green)));
    world.add(std::unique_ptr<hittable>(new quad(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue)));
    world.add(std::unique_ptr<hittable>(new quad(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange)));
    world.add(std::unique_ptr<hittable>(new quad(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal)));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 5;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    bouncing_spheres();
    //quads();
}
