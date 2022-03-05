//
// Created by 孙政 on 2022/3/5.
//

#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "rt_library.h"

class texture{
public:
    virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

class constant_texture: public texture{
public:
    constant_texture() {}
    constant_texture(vec3 c): color(c){}

    virtual vec3 value(double u, double v, const vec3& p) const {
        return color;
    }
public:
    vec3 color;
};

//棋盘格纹理
class checker_texture: public texture{
public:
    checker_texture() {}
    checker_texture(std::shared_ptr<texture> t0, std::shared_ptr<texture> t1): even(t0), odd(t1) {}

    virtual vec3 value(double u, double v, const vec3& p) const{
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if(sines < 0){
            return odd->value(u, v, p);
        }
        else{
            return even->value(u, v, p);
        }
    }
public:
    std::shared_ptr<texture> odd;   //奇数格颜色
    std::shared_ptr<texture> even;  //偶数格颜色
};

#endif //RAYTRACING_TEXTURE_H
