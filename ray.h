//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "vec3.h"

class ray{
public:
    ray() {}
    ray(const vec3& origin, const vec3& direction, double time = 0.0): orig(origin), dir(direction), tm(time) {}
    vec3 origin() const {return orig;}
    vec3 direction() const {return dir;}
    double time() const {return tm;}
    //前进t个dir后的位置
    vec3 at(double t) const {
        return orig + t * dir;
    }

public:
    vec3 orig;  //起点
    vec3 dir;   //方向
    double tm;  //光线所在的时刻
};

#endif //RAYTRACING_RAY_H
