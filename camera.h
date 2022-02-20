//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_CAMERA_H
#define RAYTRACING_CAMERA_H

#include "rt_library.h"

class camera{
public:
    camera(){
        lower_left_corner = vec3(-2, -1, -1);
        horizontal = vec3(4,0,0);
        vertical = vec3(0,2,0);
        origin = vec3(0,0,0);
    }

    ray get_ray(double u, double v){
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

#endif //RAYTRACING_CAMERA_H
