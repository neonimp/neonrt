#include "rt_info.h"

const char *rt_get_compiler()
{
	#if defined(__clang__)
	#define RT_COMPILER "Clang " __clang_version__
	#elif defined(__GNUC__)
	#define RT_COMPILER "GCC" __VERSION__
	#elif defined(_MSC_VER)
	#define RT_COMPILER "MSVC" _MSC_VER
	// Generic compiler
	#elif defined(__cplusplus)
	#define RT_COMPILER "Unknown C++ compiler"
	#else
	#define RT_COMPILER "Unknown C compiler"
	#endif // Compiler
	return RT_COMPILER;
}
