#include "rt_library.h"
#include "bvh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


vec3 ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;
    if(depth <= 0){
        return vec3(0,0,0);
    }
    if(world.hit(r, 0.001, infinity, rec)){     //这里使用0.001是为了防止自相交，从而避免阴影渗漏
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

hittable_list my_scene(){
    hittable_list world;
    //漫反射球
    world.add(std::make_shared<sphere>(vec3(0,0,-1), 0.5,
                                       std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.5, 0.5, 0.5)))));
    //金属球
    world.add(std::make_shared<sphere>(vec3(1,0,-1), 0.5,
                                       std::make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.1)));
    //玻璃球
    world.add(std::make_shared<sphere>(vec3(-1,0,-1), 0.5,
                                       std::make_shared<dielectric>(1.5)));
    //大玻璃球里套一个反向的小玻璃球，可以形成一个通透的玻璃球
    world.add(std::make_shared<sphere>(vec3(-1,0,-1), -0.45,
                                       std::make_shared<dielectric>(1.5)));
    //地面球
    world.add(std::make_shared<sphere>(vec3(0,-100.5, -1), 100,
                                       std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.5, 0.5, 0.5)))));
    return world;
}

//生成一个随机场景
hittable_list random_scene() {
    hittable_list world;
    auto checker = std::make_shared<checker_texture>(
            std::make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
            std::make_shared<constant_texture>(vec3(0.9, 0.9, 0.9))
            );  //棋盘格纹理
    world.add(make_shared<sphere>(vec3(0,-1000,0), 1000, std::make_shared<lambertian>(checker)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    world.add(make_shared<moving_sphere>(center, center + vec3(0, random_double(0, 0.5), 0), 0.0, 1.0, 0.2, std::make_shared<lambertian>(std::make_shared<constant_texture>(albedo))));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(make_shared<sphere>(center, 0.2, std::make_shared<metal>(albedo, fuzz)));
                }
                else {
                    // glass
                    world.add(make_shared<sphere>(center, 0.2, std::make_shared<dielectric>(1.5)));
                }
            }
        }
    }
    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, std::make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0, std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.5, 0.5, 0.5)))));
    world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0, std::make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));
    return static_cast<hittable_list>(std::make_shared<bvh_node>(world, 0, 1));
//    return world;
}

//生成一个两个球的场景
hittable_list two_spheres() {
    hittable_list objects;

    auto checker = std::make_shared<checker_texture>(
            std::make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
            std::make_shared<constant_texture>(vec3(0.9, 0.9, 0.9))
    );

    objects.add(std::make_shared<sphere>(vec3(0,-10, 0), 10, std::make_shared<lambertian>(checker)));
    objects.add(std::make_shared<sphere>(vec3(0, 10, 0), 10, std::make_shared<lambertian>(checker)));

    return objects;
}

//噪声测试场景
hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = std::make_shared<noise_texture>(5);
    objects.add(std::make_shared<sphere>(vec3(0,-1000, 0), 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(vec3(0, 2, 0), 2, std::make_shared<lambertian>(pertext)));

    return objects;
}
//噪声扰动测试场景
hittable_list two_turb_spheres() {
    hittable_list objects;

    auto pertext = std::make_shared<turb_texture>(5);
    objects.add(std::make_shared<sphere>(vec3(0,-1000, 0), 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(vec3(0, 2, 0), 2, std::make_shared<lambertian>(pertext)));

    return objects;
}

//图像测试场景
hittable_list image_scene(){
    int nx, ny, nn;
    unsigned char* texture_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);

    auto earth_surface =
            std::make_shared<lambertian>(std::make_shared<image_texture>(texture_data, nx, ny));
    auto globe = std::make_shared<sphere>(vec3(0,0,0), 2, earth_surface);

    return hittable_list(globe);
}


int main() {
    const int image_width = 800;
    const int image_height = 400;
    const int samples_per_pixel = 50;
    const int max_depth = 50;

    std::ofstream fout;
    fout.open("test.ppm");

    fout << "P3\n" << image_width << " " << image_height << "\n255\n";

//    hittable_list world = random_scene();
//    hittable_list world = two_spheres();
//    hittable_list world = two_perlin_spheres();
//    hittable_list world = two_turb_spheres();
    hittable_list world = image_scene();

    auto R = cos(pi/4);
    auto aspect_ratio = double(image_width)/image_height;
    vec3 vup = vec3(0,1,0);
    double fov = 20;
    vec3 lookfrom = vec3(13,2,3);
    vec3 lookat = vec3(0,1,0);
    auto dist_to_focus = (lookfrom - lookat).length();
    auto aperture = 0.1;    //光圈大小
    double shutter_begin_time = 0.0;
    double shutter_end_time = 1.0;
    camera cam(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, shutter_begin_time, shutter_end_time);

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
