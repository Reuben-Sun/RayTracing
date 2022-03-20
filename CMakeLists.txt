cmake_minimum_required(VERSION 3.20)
project(RayTracing)

set(CMAKE_CXX_STANDARD 14)

add_executable(RayTracing main.cpp vec3.h ray.h hittable.h sphere.h hittable_list.h rt_library.h camera.h material.h aabb.h moving_sphere.h bvh.h texture.h perlin.h stb_image.h xy_rect.h light.h flip_face.h box.h translate.h volumes.h)
