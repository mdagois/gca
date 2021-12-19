cmake_minimum_required(VERSION 3.10.0)
project(converter)

add_executable(gfxconv gfxconv.cpp)

find_package(libpng CONFIG REQUIRED)
target_link_libraries(gfxconv PRIVATE png)

