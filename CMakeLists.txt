cmake_minimum_required(VERSION 3.20)
project(RayTracing)

set(CMAKE_CXX_STANDARD 14)

add_executable(RayTracing main.cpp vec3.h ray.h hittable.h sphere.h hittable_list.h rt_library.h camera.h)
