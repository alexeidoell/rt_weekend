#include "camera.h"
#include "material.h"
#include "tri.h"
#include "vec3.h"
#include "color3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "common.h"
#include "sphere.h"
#include <memory>
#include <print>

// should probably change this from just using a bunch of raw pointers but
// I didn't wanna use shared pointers

void bouncing_spheres() {

    hittable_list world;

    auto ground_material = new lambertian(color3(0.5, 0.5, 0.5));
    world.add<sphere>(point3(0,-1000,0), 1000, ground_material);
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                const material* sphere_material;
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color3::random() * color3::random();
                    sphere_material = new lambertian(albedo);
                    auto center2 = center + vec3(0, random_double(0, 0.5), 0);
                    world.add<sphere>(center, center2, 0.2, sphere_material);
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = new metal(albedo, fuzz);
                    world.add<sphere>(center, 0.2, sphere_material);
                } else {
                    // glass
                    sphere_material = new dielectric(1.5);
                    world.add<sphere>(center, 0.2, sphere_material);
                }
            }
        }
    }

    auto material1 = new dielectric(1.5);
    world.add<sphere>(point3(0, 1, 0), 1.0, material1);

    auto material2 = new lambertian(color3(0.4, 0.2, 0.1));
    world.add<sphere>(point3(-4, 1, 0), 1.0, material2);

    auto material3 = new metal(color3(0.7, 0.6, 0.5), 0.0);
    world.add<sphere>(point3(4, 1, 0), 1.0, material3);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;
    cam.background_color = color3(0.70, 0.80, 1.00);

    std::vector<std::unique_ptr<bvh_node>> node_list;
    bvh_node root = bvh_node(world, node_list);
    cam.render(root);
}

void quads() {
    hittable_list world;

    // Materials
    auto left_red     = new lambertian(color3(1.0, 0.2, 0.2));
    auto back_green   = new lambertian(color3(0.2, 1.0, 0.2));
    auto right_blue   = new lambertian(color3(0.2, 0.2, 1.0));
    auto upper_orange = new lambertian(color3(1.0, 0.5, 0.0));
    auto lower_teal   = new lambertian(color3(0.2, 0.8, 0.8));

    // Quads
    world.add<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red);
    world.add<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green);
    world.add<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue);
    world.add<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange);
    world.add<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9;
    cam.image_width       = 1920;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);
    cam.background_color = color3(0.70, 0.80, 1.00);

    cam.defocus_angle = 0;

    std::vector<std::unique_ptr<bvh_node>> node_list;
    bvh_node root = bvh_node(world, node_list);
    cam.render(root);
}

void cornell_box() {
    hittable_list world;

    auto red   = new lambertian(color3(.65, .05, .05));
    auto white = new lambertian(color3(.73, .73, .73));
    auto green = new lambertian(color3(.12, .45, .15));
    auto light = new diffuse_light(color3(15, 15, 15));

    world.add<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green);
    world.add<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red);
    world.add<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light);
    world.add<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white);
    world.add<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white);
    world.add<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9;
    cam.image_width       = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);
    cam.background_color = color3(0, 0, 0);

    cam.defocus_angle = 0;

    std::vector<std::unique_ptr<bvh_node>> node_list;
    bvh_node root = bvh_node(world, node_list);
    cam.render(root);
}

int main() {
    bouncing_spheres();
    //quads();
    //cornell_box();
}
