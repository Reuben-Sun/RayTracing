//
// Created by 孙政 on 2022/3/19.
//

#ifndef RAYTRACING_BOX_H
#define RAYTRACING_BOX_H


#include "rt_library.h"

//长方体
class box: public hittable  {
public:
    box() {}
    box(const vec3& p0, const vec3& p1, std::shared_ptr<material> ptr);

    virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
        output_box = aabb(box_min, box_max);
        return true;
    }

public:
    vec3 box_min;
    vec3 box_max;
    hittable_list sides;
};

box::box(const vec3 &p0, const vec3 &p1, std::shared_ptr<material> ptr) {
    box_min = p0;
    box_max = p1;

    //六个面
    sides.add(std::make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(std::make_shared<flip_face>(std::make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr)));

    sides.add(std::make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(std::make_shared<flip_face>(std::make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)));

    sides.add(std::make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(std::make_shared<flip_face>(std::make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr)));
}

bool box::hit(const ray &r, double t0, double t1, hit_record &rec) const {
    return sides.hit(r, t0, t1, rec);
}


#endif //RAYTRACING_BOX_H
