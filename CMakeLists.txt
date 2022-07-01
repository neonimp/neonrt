cmake_minimum_required(VERSION 3.22)
project(rthost C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE RTHOST_HEADERS src/**/*.h)
file(GLOB_RECURSE RTHOST_HEADERS ${RTHOST_HEADERS} src/*.h)
file(GLOB_RECURSE RTHOST_SRC src/**/*.c)
file(GLOB_RECURSE RTHOST_SRC ${RTHOST_SRC} src/*.c)

set(xxHash_DIR ${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(NEON_CFLAGS -Wall -Wpedantic -O2)
    set(NEON_LINK_FLAGS)
endif ()

# Find xxhash and add it's static library to the resulting binary
set(XXHASH_BUILD_ENABLE_INLINE_API OFF)
set(XXHASH_BUILD_XXHSUM OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial/ ${PROJECT_BINARY_DIR}/vendor/xxhash/ EXCLUDE_FROM_ALL)

add_compile_definitions(CMAKE_RT_VERSION=1)
add_compile_definitions(RT_VERSION_MAJOR=0)
add_compile_definitions(RT_VERSION_MINOR=1)
add_compile_definitions(RT_VERSION_PATCH=0)
add_compile_definitions(RT_VERSION_STRING="0.1.0")

add_library(neonrt SHARED ${RTHOST_HEADERS} ${RTHOST_SRC})
target_include_directories(neonrt PRIVATE ${PROJECT_SOURCE_DIR}/vendor/xxhash)
target_link_libraries(neonrt PRIVATE xxHash::xxhash)
target_compile_options(neonrt PRIVATE ${NEON_CFLAGS})
target_link_options(neonrt PRIVATE ${NEON_LINK_FLAGS})

add_library(neon STATIC ${RTHOST_HEADERS} ${RTHOST_SRC})
target_include_directories(neon PRIVATE ${PROJECT_SOURCE_DIR}/vendor/xxhash)
target_link_libraries(neon PRIVATE xxHash::xxhash)
target_compile_options(neon PRIVATE ${NEON_CFLAGS})
target_link_options(neon PRIVATE ${NEON_LINK_FLAGS})

add_executable(test_guest test_guest/guest.c)
target_link_libraries(test_guest PRIVATE neonrt)
target_include_directories(test_guest PRIVATE src)

# Find jemalloc, on FreeBSD it's the default system malloc
if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
if (DEFINED JEMALLOC_ROOT_DIR)
find_package(JeMalloc)
else ()
set(JEMALLOC_FOUND true)
endif ()
elseif (DEFINED USE_JEMALLOC)
find_package(JeMalloc REQUIRED)
target_link_libraries(neonrt PRIVATE ${JEMALLOC_LIBRARIES})
target_link_libraries(neon PRIVATE ${JEMALLOC_LIBRARIES})
target_include_directories(neonrt PRIVATE ${JEMALLOC_INCLUDE_DIR})
target_include_directories(neon PRIVATE ${JEMALLOC_INCLUDE_DIR})
endif ()
