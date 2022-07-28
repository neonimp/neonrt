cmake_minimum_required(VERSION 3.22)
project(NeonLayer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE RTHOST_HEADERS src/**/*.h)
file(GLOB_RECURSE RTHOST_HEADERS ${RTHOST_HEADERS} src/*.h)
file(GLOB_RECURSE RTHOST_SRC src/**/*.c)
file(GLOB_RECURSE RTHOST_SRC ${RTHOST_SRC} src/*.c)

set(xxHash_DIR ${PROJECT_SOURCE_DIR}/vendor/xxhash/cmake_unofficial)

if (NOT ${CMAKE_C_COMPILER} MATCHES "MSVC")
    set(NEON_CFLAGS -Wall -Wpedantic -O2)
    set(NEON_LINK_FLAGS)
endif ()

add_compile_definitions(CMAKE_RT_VERSION=1)
add_compile_definitions(RT_VERSION_MAJOR=0)
add_compile_definitions(RT_VERSION_MINOR=1)
add_compile_definitions(RT_VERSION_PATCH=0)
add_compile_definitions(RT_VERSION_STRING="0.1.0")

add_subdirectory(vendor/utf8proc)
set(UTF8PROC_INSTALL off)
set(BUILD_SHARED_LIBS off)

add_library(NeonLayer SHARED ${RTHOST_HEADERS} ${RTHOST_SRC})
target_include_directories(NeonLayer PUBLIC vendor/utf8proc)
target_compile_options(NeonLayer PRIVATE ${NEON_CFLAGS})
target_link_options(NeonLayer PRIVATE ${NEON_LINK_FLAGS})
target_link_libraries(NeonLayer PRIVATE utf8proc)

add_library(Neon STATIC ${RTHOST_HEADERS} ${RTHOST_SRC})
target_include_directories(Neon PUBLIC vendor/utf8proc)
target_compile_options(Neon PRIVATE ${NEON_CFLAGS})
target_link_options(Neon PRIVATE ${NEON_LINK_FLAGS})
target_link_libraries(Neon PRIVATE utf8proc)

add_executable(test_guest test_guest/guest.c)
target_link_libraries(test_guest PRIVATE NeonLayer)
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
    target_link_libraries(NeonLayer PRIVATE ${JEMALLOC_LIBRARIES})
    target_link_libraries(Neon PRIVATE ${JEMALLOC_LIBRARIES})
    target_include_directories(NeonLayer PRIVATE ${JEMALLOC_INCLUDE_DIR})
    target_include_directories(Neon PRIVATE ${JEMALLOC_INCLUDE_DIR})
endif ()

if (DEFINED BUILD_SAMPLES)
    add_subdirectory(samples/kv_store)
endif ()
