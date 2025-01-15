cmake_minimum_required(VERSION 3.10)

project(ShadowCasting)

add_subdirectory(lib/raylib)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/raylib/raylib/include)

add_executable(shadowcast shadowcast.cpp)
target_link_libraries(shadowcast raylib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/light.jpg ${CMAKE_CURRENT_BINARY_DIR}/assets/light.jpg COPYONLY)