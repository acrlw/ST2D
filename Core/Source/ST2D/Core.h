#pragma once

#ifdef _WIN32
#define ST_PLATFORM_WINDOWS
#else
#error "This project only supports Windows!"
#endif

#ifdef ST_PLATFORM_WINDOWS
#ifdef ST_BUILD_DLL
#define ST_API __declspec(dllexport)
#else
#define ST_API __declspec(dllimport)
#endif
#else
#define ST_API
#endif

#ifdef ST_DEBUG

#define ST_ENABLE_CORE_LOGGER
#define ST_ENABLE_APP_LOGGER
#define ST_ENABLE_ASSERT
#define ST_ENABLE_TRACY_PROFILER


#else

#define ZoneScopedN(name)

#endif

#ifdef ST_ENABLE_TRACY_PROFILER

#define TRACY_ENABLE
#include "tracy/Tracy.hpp"
#endif

#include "Common.h"

namespace ST
{

}

