#pragma once

#include "Common.h"

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

#else

#endif

namespace ST
{

}

