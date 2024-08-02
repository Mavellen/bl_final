cmake_minimum_required(VERSION 3.28)
project(bl_final)

set(CMAKE_CXX_STANDARD 23)
set(raylib_path "D:/dev/stuff/raylib-source/raylib-5.0/src")

#add_compile_options(-O2)

add_executable(bl_final main.cpp
        oop_appr.cpp
        dod_appr.cpp
        max.cpp
)

target_include_directories(bl_final PRIVATE ${raylib_path})
target_link_directories(bl_final PRIVATE ${raylib_path})

target_link_libraries(bl_final raylib winmm.lib)
