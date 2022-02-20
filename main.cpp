#include <iostream>
#include <fstream>
#include "rt_library.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"


vec3 ray_color(const ray& r, const hittable& world){
    hit_record rec;
    if(world.hit(r, 0, infinity, rec)){
        return 0.5 * (rec.normal + vec3(1,1,1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1,1,1) + t * vec3(0.5, 0.7, 1.0);
}

int main() {
    const int image_width = 1000;
    const int image_height = 500;
    const int samples_per_pixel = 100;

    std::ofstream fout;
    fout.open("test.ppm");

    fout << "P3\n" << image_width << " " << image_height << "\n255\n";

    hittable_list world;
    world.add(std::make_shared<sphere>(vec3(0,0,-1), 0.5));
    world.add(std::make_shared<sphere>(vec3(0,-100.5, -1), 100));

    camera cam;

    for(int j = image_height-1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;   //显示剩余时间
        for(int i = 0; i < image_width; i++){
            vec3 color(0,0,0);
            for(int s = 0; s < samples_per_pixel; s++){
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u,v);
                color += ray_color(r, world);
            }
            color.write_color(fout, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
    fout.close();
    return 0;
}
