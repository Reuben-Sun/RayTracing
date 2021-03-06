//
// Created by 孙政 on 2022/3/5.
//

#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "rt_library.h"
#include "perlin.h"

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

//噪声纹理
class noise_texture: public texture{
public:
    noise_texture() {}
    noise_texture(double sc): scale(sc) {}

    virtual vec3 value(double u, double v, const vec3& p) const{
        return vec3(1,1,1) * 0.5 * (1 + noise.noise(p * scale));
    }
public:
    perlin noise;
    double scale;   //噪声频率
};

//扰动纹理
class turb_texture: public texture{
public:
    turb_texture() {}
    turb_texture(double sc): scale(sc) {}

    virtual vec3 value(double u, double v, const vec3& p) const{
        return vec3(1,1,1) * 0.5 * (1 + sin(scale * p.z() + 5 * noise.turb(p)));
    }
public:
    perlin noise;
    double scale;   //噪声频率;
};

//图片纹理
class image_texture: public texture{
public:
    image_texture() {}
    image_texture(unsigned char* pixels, int A, int B): data(pixels), nx(A), ny(B) {}
    ~image_texture(){
        delete data;
    }

    virtual vec3 value(double u, double v, const vec3& p) const{
        if(data == nullptr){
            return vec3(0,1,1);
        }

        auto i = static_cast<int>(u * nx);
        auto j = static_cast<int>((1-v) * ny - 0.001);

        if(i < 0) i = 0;
        if(j < 0) j = 0;
        if(i > nx-1) i = nx -1;
        if(j > ny-1) j = ny -1;

        auto r = static_cast<int>(data[3*i + 3*nx*j + 0]) / 255.0;
        auto g = static_cast<int>(data[3*i + 3*nx*j + 0]) / 255.0;
        auto b = static_cast<int>(data[3*i + 3*nx*j + 0]) / 255.0;

        return vec3(r, g, b);
    }
public:
    unsigned char* data;
    int nx, ny;
};
#endif //RAYTRACING_TEXTURE_H
