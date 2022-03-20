#include "rt_library.h"
#include "bvh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "xy_rect.h"
#include "flip_face.h"
#include "box.h"
#include "translate.h"
#include "volumes.h"


vec3 ray_color(const ray& r, const vec3& background, const hittable& world, int depth){
    hit_record rec;
    if(depth <= 0){
        return vec3(0,0,0);
    }
    if(!world.hit(r, 0.001, infinity, rec)){     //这里使用0.001是为了防止自相交，从而避免阴影渗漏
        return background;
    }
    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if(!rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
        return emitted;
    }
    return emitted + attenuation * ray_color(scattered, background, world, depth -1);
//    vec3 unit_direction = unit_vector(r.direction());
//    auto t = 0.5 * (unit_direction.y() + 1.0);
//    return (1.0 - t) * vec3(1,1,1) + t * vec3(0.5, 0.7, 1.0);
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

//矩形光源测试场景
hittable_list simple_light() {
    hittable_list objects;

    auto pertext = std::make_shared<turb_texture>(4);
    objects.add(std::make_shared<sphere>(vec3(0,-1000, 0), 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(vec3(0,2,0), 2, std::make_shared<lambertian>(pertext)));

    auto difflight = std::make_shared<diffuse_light>(std::make_shared<constant_texture>(vec3(4,4,4)));
    objects.add(std::make_shared<sphere>(vec3(0,7,0), 2, difflight));
    objects.add(std::make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

//Cornell Box场景
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
    auto white = make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    auto green = make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
    auto light = make_shared<diffuse_light>(std::make_shared<constant_texture>(vec3(15, 15, 15)));

    //背景
    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));
    //物体
    std::shared_ptr<hittable> box1 = std::make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = std::make_shared<rotate_y>(box1,  15);
    box1 = std::make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    std::shared_ptr<hittable> box2 = std::make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);
    return objects;
}

//Cornell Smoke场景
hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
    auto white = std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    auto green = std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
    auto light = std::make_shared<diffuse_light>(std::make_shared<constant_texture>(vec3(7, 7, 7)));

    objects.add(std::make_shared<flip_face>(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(std::make_shared<flip_face>(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<flip_face>(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    std::shared_ptr<hittable> box1 = std::make_shared<box>(vec3(0,0,0), vec3(165,330,165), white);
    box1 = std::make_shared<rotate_y>(box1,  15);
    box1 = std::make_shared<translate>(box1, vec3(265,0,295));

    std::shared_ptr<hittable> box2 = std::make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, vec3(130,0,65));

    objects.add(std::make_shared<constant_medium>(box1, 0.01, std::make_shared<constant_texture>(vec3(0,0,0))));
    objects.add(std::make_shared<constant_medium>(box2, 0.01, std::make_shared<constant_texture>(vec3(1,1,1))));

    return objects;
}

int main() {
    const int image_width = 400;
    const int image_height = 400;
    const int samples_per_pixel = 200;
    const int max_depth = 200;

    std::ofstream fout;
    fout.open("test.ppm");

    fout << "P3\n" << image_width << " " << image_height << "\n255\n";

//    hittable_list world = random_scene();
//    hittable_list world = two_spheres();
//    hittable_list world = two_perlin_spheres();
//    hittable_list world = two_turb_spheres();
//    hittable_list world = image_scene();
//    hittable_list world = cornell_box();
    hittable_list world = cornell_smoke();

    auto R = cos(pi/4);
    auto aspect_ratio = double(image_width)/image_height;
    vec3 vup = vec3(0,1,0);
    double fov = 40;
    vec3 lookfrom = vec3(278,278,-800);
    vec3 lookat = vec3(278,278,0);
//    auto dist_to_focus = (lookfrom - lookat).length();
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;    //光圈大小
    double shutter_begin_time = 0.0;
    double shutter_end_time = 1.0;
    camera cam(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, shutter_begin_time, shutter_end_time);

    const vec3 background(0,0,0);   //纯黑背景

    for(int j = image_height-1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;   //显示剩余时间
        for(int i = 0; i < image_width; i++){
            vec3 color(0,0,0);
            for(int s = 0; s < samples_per_pixel; s++){
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u,v);
                color += ray_color(r, background, world, max_depth);
            }
            color.write_color(fout, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
    fout.close();
    return 0;
}
