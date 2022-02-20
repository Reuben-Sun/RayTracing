#include <iostream>
#include <fstream>
#include "rt_library.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


vec3 ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;
    if(depth <= 0){
        return vec3(0,0,0);
    }
    if(world.hit(r, 0.001, infinity, rec)){     //这里使用0.001是为了防止自相交，从而避免阴影渗漏
//        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
//        vec3 target = rec.p + rec.normal + random_unit_vector();
//        vec3 target = rec.p + random_in_hemisphere(rec.normal);
        ray scattered;
        vec3 attenuation;
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
            return attenuation * ray_color(scattered, world, depth -1);
        }
        return vec3(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1,1,1) + t * vec3(0.5, 0.7, 1.0);
}

int main() {
    const int image_width = 400;
    const int image_height = 200;
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    std::ofstream fout;
    fout.open("test.ppm");

    fout << "P3\n" << image_width << " " << image_height << "\n255\n";

    hittable_list world;
    //漫反射球
    world.add(std::make_shared<sphere>(vec3(0,0,-1), 0.5,
            std::make_shared<lambertian>(vec3(0.1, 0.2, 0.5))));
    //金属球
    world.add(std::make_shared<sphere>(vec3(1,0,-1), 0.5,
            std::make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.1)));
    //玻璃球
    world.add(std::make_shared<sphere>(vec3(-1,0,-1), 0.5,
            std::make_shared<dielectric>(1.5)));
    //地面球
    world.add(std::make_shared<sphere>(vec3(0,-100.5, -1), 100,
            std::make_shared<metal>(vec3(0.8, 0.8, 0.8), 0.5)));
    camera cam;

    for(int j = image_height-1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;   //显示剩余时间
        for(int i = 0; i < image_width; i++){
            vec3 color(0,0,0);
            for(int s = 0; s < samples_per_pixel; s++){
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u,v);
                color += ray_color(r, world, max_depth);
            }
            color.write_color(fout, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
    fout.close();
    return 0;
}
