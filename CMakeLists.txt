cmake_minimum_required(VERSION 3.7)

project(tracer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(kismet)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost COMPONENTS program_options REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "set build type to Release")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Release AND NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

file(GLOB HEADERS *.h)
source_group("Headers" FILES ${HEADERS})

file(GLOB SOURCES *.cpp)

add_executable(tracer
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(tracer
    PRIVATE kismet/include ctpl ${Boost_INCLUDE_DIRS})
target_link_libraries(tracer
    PRIVATE math ${Boost_LIBRARIES})
target_compile_features(tracer PRIVATE cxx_std_17)

# copy the earthmap texture to the output directory
# We cannot use generator expressions in OUTPUT, instead, create a timestamp file
# and copy the texture at the same time. This has the limitation that
# if the target texture is removed, it won't be created.
add_custom_command(
    OUTPUT earthmap.ts
    COMMAND ${CMAKE_COMMAND} -E touch earthmap.ts
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/earthmap.jpg $<TARGET_FILE_DIR:tracer>
    DEPENDS ${CMAKE_SOURCE_DIR}/earthmap.jpg
    COMMENT "copy earthmap.jpg"
    VERBATIM)
add_custom_target(earthmap ALL DEPENDS earthmap.ts)
