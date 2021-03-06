//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_RT_LIBRARY_H
#define RAYTRACING_RT_LIBRARY_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>


const double infinity = std::numeric_limits<double>::infinity();    //无穷
const double pi = 3.1415926535897932385;    //PI

//度数转化为弧度
inline double degrees_to_radians(double degrees){
    return degrees * pi / 180;
}

//两者取小
inline double ffmin(double a, double b) {return a <= b ? a : b;}
//两者取大
inline double ffmax(double a, double b) {return a >= b ? a : b;}

//返回随机数，[0,1)
inline double random_double(){
    return rand() / (RAND_MAX + 1.0);
}
//返回随机数，[min,max)
inline double random_double(double min, double max){
    return min + (max-min) * random_double();
}
//返回随机数，[min,max)
inline int random_int(int min, int max){
    return min + (max-min) * (int)random_double();
}

//将x限制在[min,max]
inline double clamp(double x, double min, double max){
    if(x < min) return min;
    if(x > max) return max;
    return x;
}

//菲涅尔
double schlick(double cosine, double ref_idx){
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow(1-cosine, 5);
}

#include "hittable_list.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"


#endif //RAYTRACING_RT_LIBRARY_H
