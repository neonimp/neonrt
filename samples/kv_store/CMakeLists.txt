cmake_minimum_required(VERSION 3.22)
project(KVStore C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE KV_STORE_SRC *.c)
file(GLOB_RECURSE KV_STORE_HEADERS *.h)

add_executable(kv_store ${KV_STORE_SRC} ${KV_STORE_HEADERS})
target_include_directories(kv_store PUBLIC ../../src)
target_link_libraries(kv_store PRIVATE NeonLayer)
