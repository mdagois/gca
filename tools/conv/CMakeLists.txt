cmake_minimum_required(VERSION 3.10.0)
project(converter)

# DMG converter
add_executable(gfxconv gfxconv.cpp)
target_compile_definitions(gfxconv PUBLIC _CRT_SECURE_NO_WARNINGS)
find_package(libpng CONFIG REQUIRED)
target_link_libraries(gfxconv PRIVATE png)

# CGB converter
add_executable(cgbconv cgbconv.cpp)
target_compile_definitions(cgbconv PUBLIC _CRT_SECURE_NO_WARNINGS)
find_package(libpng CONFIG REQUIRED)
target_link_libraries(cgbconv PRIVATE png)

# Packetizer
add_executable(packetize packetize.cpp)
target_compile_definitions(packetize PUBLIC _CRT_SECURE_NO_WARNINGS)

# SGB converter
add_executable(sgbconv sgbconv.cpp)
target_compile_definitions(sgbconv PUBLIC _CRT_SECURE_NO_WARNINGS)
find_package(libpng CONFIG REQUIRED)
target_link_libraries(sgbconv PRIVATE png)

