//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H



//球
class sphere: public hittable{
public:
    sphere() {}
    sphere(vec3 cen, double r, std::shared_ptr<material> m): center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb &output_box) const;

public:
    vec3 center;
    double radius;
    std::shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;    //求根公式，(b^2 - 4ac)/4

    if(discriminant > 0){
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root)/a;     //第一个解
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-half_b + root)/a;      //第二个解
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool sphere::bounding_box(double t0, double t1, aabb &output_box) const {
    output_box = aabb(
            center - vec3(radius, radius, radius),
            center + vec3(radius, radius, radius)
            );
    return true;
}

//获得球的uv
void get_sphere_uv(const vec3& p, double& u, double& v){
    auto phi = atan2(p.z(), p.x());     //极坐标相对极轴朝下旋转的角度
    auto theta = asin(p.y());           //极坐标绕着极轴旋转的角度
    u = 1 - (phi + pi) / (2 * pi);
    v = (theta + pi/2) / pi;
}
#endif //RAYTRACING_SPHERE_H
