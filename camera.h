//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_CAMERA_H
#define RAYTRACING_CAMERA_H

#include "rt_library.h"

class camera{
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup,double vfov, double aspect, double aperture, double focus_dist){
        origin = lookfrom;      //lookfrom是从哪看
        lens_radius = aperture/2;

        auto theta = degrees_to_radians(vfov);      //fov是视场角
        auto half_height = tan(theta/2);
        auto half_width = aspect * half_height;

        w = unit_vector(lookfrom - lookat);     //获得观察向量，lookat是看向哪
        u = unit_vector(cross(vup, w));         //点积重建相机空间(获得右向量)
        v = cross(w, u);    //重算上向量

        lower_left_corner = origin
                - half_width * u * focus_dist
                - half_height * v * focus_dist
                - w * focus_dist;
        horizontal = 2 * half_width * u * focus_dist;
        vertical = 2 * half_height * v * focus_dist;
    }

    ray get_ray(double s, double t){
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;     //相机直径（用于景深）
};

#endif //RAYTRACING_CAMERA_H
