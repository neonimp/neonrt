#pragma once

#ifndef RT_INFO_H
#define RT_INFO_H

#ifndef CMAKE_RT_VERSION
#define RT_VERSION_MAJOR 0
#define RT_VERSION_MINOR 0
#define RT_VERSION_PATCH 0
#define RT_VERSION_STRING "0.0.0"
#endif // CMAKE_RT_VERSION

extern const char* rt_get_compiler();

#endif // RT_INFO_H
