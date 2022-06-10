cmake_minimum_required(VERSION 3.22)
project(rthost C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE RTHOST_HEADERS src/**/*.h)
file(GLOB_RECURSE RTHOST_SRC src/**/*.c)

set(xxHash_DIR ${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial)

# Find jemalloc
if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    if (DEFINED JEMALLOC_ROOT_DIR)
        find_package(JeMalloc)
    else ()
        set(JEMALLOC_FOUND true)
    endif ()
else ()
    find_package(JeMalloc REQUIRED)
endif ()

# Find xxhash and add it's static library to the resulting binary
set(XXHASH_BUILD_ENABLE_INLINE_API OFF)
set(XXHASH_BUILD_XXHSUM OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial/ ${PROJECT_BINARY_DIR}/vendor/xxhash/ EXCLUDE_FROM_ALL)

add_library(neonrt SHARED ${RTHOST_HEADERS} ${RTHOST_SRC})
target_include_directories(neonrt PRIVATE ${PROJECT_SOURCE_DIR}/vendor/xxhash)
target_include_directories(neonrt PRIVATE ${JEMALLOC_INCLUDE_DIR})

target_link_libraries(neonrt PRIVATE xxHash::xxhash)

add_executable(test_guest test_guest/guest.c)
target_link_libraries(test_guest PRIVATE neonrt)
target_include_directories(test_guest PRIVATE src)
