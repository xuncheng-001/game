cmake_minimum_required(VERSION 3.30)
project(zvp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(SDL)
add_subdirectory(SDL_image)
add_subdirectory(libpng)

add_executable(zvp main.cpp)

target_link_libraries(zvp PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        png
)

target_include_directories(zvp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL_image/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libpng
)

