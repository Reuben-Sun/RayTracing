//
// Created by 孙政 on 2022/3/19.
//

#ifndef RAYTRACING_VOLUMES_H
#define RAYTRACING_VOLUMES_H

#include "rt_library.h"

//体积体
class constant_medium: public hittable{
public:
    constant_medium(std::shared_ptr<hittable> b, double d, std::shared_ptr<texture> a): boundary(b), neg_inv_density(-1/d)
    {
        phase_function = std::make_shared<isotropic>(a);
    }

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
        return boundary->bounding_box(t0, t1, output_box);
    }

public:
    std::shared_ptr<hittable> boundary;
    std::shared_ptr<material> phase_function;
    double neg_inv_density;
};

bool constant_medium::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    const bool enableDebug = false;
    const bool debugging = enableDebug && random_double() < 0.00001;

    hit_record rec1, rec2;
    if(!boundary->hit(r, -infinity, infinity, rec1)){
        return false;
    }
    if(!boundary->hit(r, rec1.t + 0.0001, infinity, rec2)){
        return false;   //这里意味着射线只会射入物体一次，不会射入第二次，即物体是一个凸几何体
    }
    if(debugging){
        std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << "\n";
    }
    if(rec1.t < t_min){
        rec1.t = t_min;
    }
    if(rec2.t > t_max){
        rec2.t = t_max;
    }
    if(rec1.t >= rec2.t){
        return false;
    }
    if(rec1.t < 0){
        rec1.t = 0;
    }

    const auto ray_length = r.direction().length();
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const auto hit_distance = neg_inv_density * log(random_double());

    if(hit_distance > distance_inside_boundary){
        return false;
    }
    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if(debugging){
        std::cerr << "hit_distance = " <<  hit_distance << '\n'
                  << "rec.t = " <<  rec.t << '\n'
                  << "rec.p = " <<  rec.p << '\n';
    }

    rec.normal = vec3(1,0,0);
    rec.front_face = true;
    rec.mat_ptr = phase_function;
    return true;
}
#endif //RAYTRACING_VOLUMES_H
