cmake_minimum_required(VERSION 3.27)

# Project Name
project(GP1_Raytracer)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(project)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(project/tests)
endif()


# REDUNDANT, use this only if you want to let CMake build SDL
# include(FetchContent)
# FetchContent_Declare(
#  SDL2
#  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#  GIT_TAG release-2.30.3
#  GIT_SHALLOW TRUE
#  GIT_PROGRESS TRUE
#)
#FetchContent_MakeAvailable(SDL2)
