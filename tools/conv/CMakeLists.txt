cmake_minimum_required(VERSION 3.10.0)
project(converter)

# DMG converter
add_executable(gfxconv gfxconv.cpp)
find_package(libpng CONFIG REQUIRED)
target_link_libraries(gfxconv PRIVATE png)

# CGB converter
add_executable(cgbconv cgbconv.cpp)
find_package(libpng CONFIG REQUIRED)
target_link_libraries(cgbconv PRIVATE png)

