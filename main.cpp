#include <iostream>
#include <fstream>
#include "rt_library.h"
#include "hittable_list.h"
#include "sphere.h"


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
    std::ofstream fout;
    fout.open("test.ppm");

    fout << "P3\n" << image_width << " " << image_height << "\n255\n";

    vec3 lower_left_corner(-2, -1, -1);
    vec3 horizontal(4,0,0);
    vec3 vertical(0,2,0);
    vec3 origin(0,0,0);

    hittable_list world;
    world.add(std::make_shared<sphere>(vec3(0,0,-1), 0.5));
    world.add(std::make_shared<sphere>(vec3(0,-100.5, -1), 100));

    for(int j = image_height-1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;   //显示剩余时间
        for(int i = 0; i < image_width; i++){
           auto u = double(i) / image_width;
           auto v = double(j) / image_height;
           ray r(origin, lower_left_corner + u * horizontal + v * vertical);
           vec3 color = ray_color(r, world);
           color.write_color(fout);
        }
    }
    std::cerr << "\nDone.\n";
    fout.close();
    return 0;
}
