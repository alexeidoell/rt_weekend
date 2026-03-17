cmake_minimum_required(VERSION 3.15...4.1)



project(rt_weekend VERSION 0.1 LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(rt_weekend
    src/main.cpp
    src/camera.cpp
    src/hittable.cpp
)

target_compile_features(rt_weekend PUBLIC cxx_std_20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(rt_weekend PRIVATE Threads::Threads)
add_subdirectory(tiny-optional)
target_link_libraries(rt_weekend PRIVATE tiny-optional::tiny-optional)
