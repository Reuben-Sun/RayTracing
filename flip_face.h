//
// Created by 孙政 on 2022/3/19.
//

#ifndef RAYTRACING_FLIP_FACE_H
#define RAYTRACING_FLIP_FACE_H

#include "rt_library.h"

//用于防止矩形翻转（因为目前是单面渲染的）
class flip_face: public hittable{
public:
    flip_face(std::shared_ptr<hittable> p): ptr(p) {}
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
        if (!ptr->hit(r, t_min, t_max, rec)){
            return false;
        }
        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
        return ptr->bounding_box(t0, t1, output_box);
    }


public:
    std::shared_ptr<hittable> ptr;
};

#endif //RAYTRACING_FLIP_FACE_H
