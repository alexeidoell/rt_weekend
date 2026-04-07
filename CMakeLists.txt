cmake_minimum_required(VERSION 3.15...4.1)



project(rt_weekend VERSION 0.1 LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_executable(rt_weekend
    src/main.cpp
    src/vec3.cpp
    src/camera.cpp
    src/sphere.cpp
    src/tri.cpp
    src/material.cpp
    src/hittable.cpp
    src/vectorization.cpp
)

target_compile_features(rt_weekend PUBLIC cxx_std_23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(highway REQUIRED)
find_package(tiny-optional REQUIRED)
target_compile_options(rt_weekend PRIVATE -march=native -mfma -fno-exceptions)
target_link_libraries(rt_weekend PRIVATE tiny-optional::tiny-optional Threads::Threads highway::highway)

