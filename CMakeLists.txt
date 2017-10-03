cmake_minimum_required(VERSION 3.7)

project(tracer)

add_subdirectory(kismet)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "set build type to Release")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

file(GLOB HEADERS *.h)
source_group("Headers" FILES ${HEADERS})

add_executable(tracer
    camera.cpp
    dielectric.cpp
    lambertian.cpp
    main.cpp
    metal.cpp
    sphere.cpp
    utils.cpp
    vec.cpp
    world.cpp
    ${HEADERS}
)

target_include_directories(tracer PRIVATE kismet/include)
target_link_libraries(tracer PRIVATE math)
target_compile_features(tracer PRIVATE cxx_std_14)

option(GAMMA "encode ppm with gamma correction" ON)
if (GAMMA)
    target_compile_definitions(tracer PRIVATE GAMMA)
endif()

option(RANDOM_SCENE "random scene" ON)
if (RANDOM_SCENE)
    target_compile_definitions(tracer PRIVATE RANDOM_SCENE)
endif()

