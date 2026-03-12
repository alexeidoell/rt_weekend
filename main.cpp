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

    auto material_ground = std::make_shared<lambertian>(color3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color3(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<dielectric>(1.5);
    auto material_right  = std::make_shared<metal>(color3(0.8, 0.6, 0.2), 0.3);

    world.add(std::unique_ptr<hittable>(new sphere(point3(0,0, -1), 0.5, material_center)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(-1,0, -1), 0.5, material_left)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(1,0, -1), 0.5, material_right)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(0,-100.5, -1), 100, material_ground)));

    camera cam;

    cam.render(world);
}
