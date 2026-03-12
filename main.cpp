#include <iostream>
#include <memory>
#include <optional>
#include "camera.h"
#include "vec3.h"
#include "ray.h"
#include "color3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "common.h"
#include "sphere.h"

int main() {

    hittable_list world;

    /*
    auto material_ground = std::make_shared<lambertian>(color3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color3(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<dielectric>(1.5);
    auto material_bubble   = std::make_shared<dielectric>(1.00/1.5);
    auto material_right  = std::make_shared<metal>(color3(0.8, 0.6, 0.2), 0.1);

    world.add(std::unique_ptr<hittable>(new sphere(point3(0,0, -1.2), 0.5, material_center)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(-1,0, -1), 0.5, material_left)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(1,0, -1), 0.5, material_right)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(-1,0, -1), 0.4, material_bubble)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(0,-100.5, -1), 100, material_ground)));

    auto R = std::cos(pi/4);

    auto material_left  = std::make_shared<lambertian>(color3(0,0,1));
    auto material_right = std::make_shared<lambertian>(color3(1,0,0));

    world.add(std::unique_ptr<hittable>(new sphere(point3(-R,0, -1), R, material_left)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(R,0, -1), R, material_right)));
    */

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

    cam.render(world);
}
