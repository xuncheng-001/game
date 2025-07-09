cmake_minimum_required(VERSION 3.0)
project(zvp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(SDL2-2.30.0)
add_subdirectory(SDL2_image-2.8.0)

add_executable(zvp main.cpp)

target_link_libraries(zvp PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
)

target_include_directories(zvp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL2-2.30.0/include
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image-2.8.0/include
)

