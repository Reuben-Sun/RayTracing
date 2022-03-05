//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_HITTABLE_H
#define RAYTRACING_HITTABLE_H

#include "ray.h"
#include "aabb.h"

class material;
//交点
struct hit_record{
    vec3 p;         //交点坐标
    vec3 normal;    //交点法线，[-1,1]
    double t;       //射线方程t值
    double u, v;    //纹理uv坐标
    bool front_face;    //入射面是否为外侧，外侧为true
    std::shared_ptr<material> mat_ptr;      //材质

    inline void set_face_normal(const ray& r, const vec3& outward_normal){
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal: -outward_normal;
    }
};

//可以被光线求交的物体
class hittable{
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
};

#endif //RAYTRACING_HITTABLE_H
