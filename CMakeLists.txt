cmake_minimum_required(VERSION 3.22)
project(rthost C)

set(CMAKE_C_STANDARD 90)

file(GLOB_RECURSE RTHOST_HEADERS src/**/*.h)
file(GLOB_RECURSE RTHOST_SRC src/**/*.c)

set(xxHash_DIR ${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial)
set(XXHASH_BUILD_ENABLE_INLINE_API OFF) #optional
set(XXHASH_BUILD_XXHSUM OFF) #optional
add_subdirectory( ${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial/ ${PROJECT_BINARY_DIR}/vendor/xxhash/ EXCLUDE_FROM_ALL)

add_library(neonrt SHARED ${RTHOST_HEADERS} ${RTHOST_SRC})
target_link_libraries(neonrt PRIVATE xxHash::xxhash)
