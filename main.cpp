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

    world.add(std::unique_ptr<hittable>(new sphere(point3(0,0, -1), 0.5)));
    world.add(std::unique_ptr<hittable>(new sphere(point3(0,-100.5, -1), 100)));

    camera cam;

    cam.render(world);
}
