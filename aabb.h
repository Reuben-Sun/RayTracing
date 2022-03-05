//
// Created by 孙政 on 2022/3/5.
//

#ifndef RAYTRACING_AABB_H
#define RAYTRACING_AABB_H

#include "vec3.h"

//aabb就是轴向最小包围盒
class aabb{
public:
    aabb() {}
    aabb(const vec3& a, const vec3& b) {_min = a; _max = b;}

    vec3 min() const {return _min;}
    vec3 max() const {return _max;}

    /* 这个方法可行，但并不好
    bool hit(const ray& r, double tmin, double tmax) const{
        for(int a = 0; a < 3; a++){
            //一个立方体的平行的两个平面将空间分成了三部分
            auto t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
                            (_max[a] - r.origin()[a] / r.direction()[a]));  //这是射线与入射平面的交点（如果有）
            auto t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
                            (_max[a] - r.origin()[a] / r.direction()[a]));  //这是射线与出射平面的交点（如果有）
            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            //如何这个射线与两个平面都相交，那么出射点是比入射点要远的，不然就是不相交
            if(tmax <= tmin){
                return false;
            }
        }
        //如果这个射线在三个方向上都相交，那么这个射线与这个AABB是相交的
        return true;
    }
    */

    bool hit(const ray& r, double tmin, double tmax) const{
        for(int a = 0; a < 3; a++){
            auto invD = 1.0f / r.direction()[a];
            auto t0 = (min()[a] - r.origin()[a]) * invD;
            auto t1 = (max()[a] - r.origin()[a]) * invD;
            if(invD < 0.0f){
                std::swap(t0, t1);
            }
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if(tmax <= tmin){
                return false;
            }
        }
        return true;
    }
public:
    vec3 _min;
    vec3 _max;
};

//计算包围盒的包围盒
aabb surrounding_box(aabb box0, aabb box1){
    vec3 small (ffmin(box0.min().x(), box1.min().x()),
                ffmin(box0.min().y(), box1.min().y()),
                ffmin(box0.min().z(), box1.min().z()));
    vec3 big (ffmax(box0.max().x(), box1.max().x()),
              ffmax(box0.max().y(), box1.max().y()),
              ffmax(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}
#endif //RAYTRACING_AABB_H
