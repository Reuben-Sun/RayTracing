//
// Created by 孙政 on 2022/2/20.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H


#include "texture.h"

class material{
public:
    virtual vec3 emitted(double u, double v, const vec3& p) const{
        return vec3(0,0,0);
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

//漫反射材质
class lambertian: public material{
public:
    lambertian(std::shared_ptr<texture> a): albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const{
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
public:
    std::shared_ptr<texture> albedo;
};

//金属材质
class metal: public material{
public:
    metal(const vec3& a, double f): albedo(a), roughness(f < 1 ? f : 1) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const{
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + roughness * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
public:
    vec3 albedo;
    double roughness;    //粗糙度
};

//玻璃材质
class dielectric: public material{
public:
    dielectric(double ri): ref_idx(ri) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        attenuation = vec3(1,1,1);
        double etai_over_etat = (rec.front_face) ? (1.0/ref_idx) : (ref_idx);

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if(etai_over_etat * sin_theta > 1.0){   //全内反射
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.p, reflected);
            return true;
        }
        double reflect_prob = schlick(cos_theta, etai_over_etat);
        if(random_double() < reflect_prob){
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.p, reflected);
            return true;
        }
        vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
        scattered = ray(rec.p, refracted);
        return true;
    }

public:
    double ref_idx;
};

//发光材质
class diffuse_light : public material{
public:
    diffuse_light(std::shared_ptr<texture> a): emit(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const{
        return false;
    }

    virtual vec3 emitted(double u, double v, const vec3& p) const{
        return emit->value(u,v,p);
    }
public:
    std::shared_ptr<texture> emit;
};
#endif //RAYTRACING_MATERIAL_H
