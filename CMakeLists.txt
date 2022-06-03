cmake_minimum_required(VERSION 3.22)
project(rthost C)

set(CMAKE_C_STANDARD 90)

file(GLOB RTHOST_HEADERS src/*.h)
file(GLOB_RECURSE RTHOST_SRC src/**/*.c)

add_library(rthost SHARED ${RTHOST_HEADERS} ${RTHOST_SRC})
