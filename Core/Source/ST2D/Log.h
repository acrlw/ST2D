#pragma once

#include "Core.h"

namespace ST
{
	class ST_API Log
	{
	public:
		static void init();
		static void destroy();

		template<typename... Args>
		static void print(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum level,
			const std::string& tag, const std::string& message, Args&&... args)
		{
			if (logger == nullptr)
				return;

			std::string prefix = tag.empty() ? " " : fmt::format("[{}] ", tag);

			switch (level)
			{
			case spdlog::level::trace:
				logger->trace(fmt::runtime(prefix + message), std::forward<Args>(args)...);
				break;
			case spdlog::level::debug:
				logger->debug(fmt::runtime(prefix + message), std::forward<Args>(args)...);
				break;
			case spdlog::level::info:
				logger->info(fmt::runtime(prefix + message), std::forward<Args>(args)...);
				break;
			case spdlog::level::warn:
				logger->warn(fmt::runtime(prefix + message), std::forward<Args>(args)...);
				break;
			case spdlog::level::err:
				logger->error(fmt::runtime(prefix + message), std::forward<Args>(args)...);
				break;
			case spdlog::level::critical:
				logger->critical(fmt::runtime(prefix + message), std::forward<Args>(args)...);
				break;
			default:
				break;
			}
		}

		static std::shared_ptr<spdlog::logger>& coreLogger() { return s_coreLogger; }
		static std::shared_ptr<spdlog::logger>& appLogger() { return s_appLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_appLogger;
	};


}

#ifdef ST_ENABLE_CORE_LOGGER
#define CORE_TRACE_TAG(tag, ...)      ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::trace, tag, __VA_ARGS__);
#define CORE_DEBUG_TAG(tag, ...)      ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::debug, tag, __VA_ARGS__);
#define CORE_INFO_TAG(tag, ...)       ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::info, tag, __VA_ARGS__);
#define CORE_WARN_TAG(tag, ...)       ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::warn, tag, __VA_ARGS__);
#define CORE_ERROR_TAG(tag, ...)      ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::err, tag, __VA_ARGS__);
#define CORE_CRITICAL_TAG(tag, ...)   ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::critical, tag, __VA_ARGS__);

#define CORE_TRACE(...)               ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::trace, "" , __VA_ARGS__);
#define CORE_DEBUG(...)               ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::debug, "" , __VA_ARGS__);
#define CORE_INFO(...)                ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::info, "" , __VA_ARGS__);
#define CORE_WARN(...)                ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::warn, "" , __VA_ARGS__);
#define CORE_ERROR(...)               ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::err, "" , __VA_ARGS__);
#define CORE_CRITICAL(...)            ::ST::Log::print(ST::Log::coreLogger(), spdlog::level::critical, "" , __VA_ARGS__);
#else
#define CORE_TRACE_TAG(tag, ...)
#define CORE_DEBUG_TAG(tag, ...)
#define CORE_INFO_TAG(tag, ...)
#define CORE_WARN_TAG(tag, ...)
#define CORE_ERROR_TAG(tag, ...)
#define CORE_CRITICAL_TAG(tag, ...)

#define CORE_TRACE(...)
#define CORE_DEBUG(...)
#define CORE_INFO(...)
#define CORE_WARN(...)
#define CORE_ERROR(...)
#define CORE_CRITICAL(...)
#endif

#ifdef ST_ENABLE_APP_LOGGER
#define APP_TRACE_TAG(tag, ...)       ::ST::Log::print(ST::Log::appLogger(), spdlog::level::trace, tag, __VA_ARGS__);
#define APP_DEBUG_TAG(tag, ...)       ::ST::Log::print(ST::Log::appLogger(), spdlog::level::debug, tag, __VA_ARGS__);
#define APP_INFO_TAG(tag, ...)        ::ST::Log::print(ST::Log::appLogger(), spdlog::level::info, tag, __VA_ARGS__);
#define APP_WARN_TAG(tag, ...)        ::ST::Log::print(ST::Log::appLogger(), spdlog::level::warn, tag, __VA_ARGS__);
#define APP_ERROR_TAG(tag, ...)       ::ST::Log::print(ST::Log::appLogger(), spdlog::level::err, tag, __VA_ARGS__);
#define APP_CRITICAL_TAG(tag, ...)    ::ST::Log::print(ST::Log::appLogger(), spdlog::level::critical, tag, __VA_ARGS__);

#define APP_TRACE(...)			      ::ST::Log::print(ST::Log::appLogger(), spdlog::level::trace, "" , __VA_ARGS__);
#define APP_DEBUG(...)			      ::ST::Log::print(ST::Log::appLogger(), spdlog::level::debug, "" , __VA_ARGS__);
#define APP_INFO(...)			      ::ST::Log::print(ST::Log::appLogger(), spdlog::level::info, "" , __VA_ARGS__);
#define APP_WARN(...)			      ::ST::Log::print(ST::Log::appLogger(), spdlog::level::warn, "" , __VA_ARGS__);
#define APP_ERROR(...)			      ::ST::Log::print(ST::Log::appLogger(), spdlog::level::err, "" , __VA_ARGS__);
#define APP_CRITICAL(...)		      ::ST::Log::print(ST::Log::appLogger(), spdlog::level::critical, "" , __VA_ARGS__);
#else
#define APP_TRACE_TAG(tag, ...)
#define APP_DEBUG_TAG(tag, ...)
#define APP_INFO_TAG(tag, ...)
#define APP_WARN_TAG(tag, ...)
#define APP_ERROR_TAG(tag, ...)
#define APP_CRITICAL_TAG(tag, ...)

#define APP_TRACE(...)
#define APP_DEBUG(...)
#define APP_INFO(...)
#define APP_WARN(...)
#define APP_ERROR(...)
#define APP_CRITICAL(...)
#endif

#ifdef ST_ENABLE_ASSERT
#define CORE_ASSERT(x, ...) { if(!(x)) { CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CORE_ASSERT_TAG(x, tag, ...) { if(!(x)) { CORE_ERROR_TAG(tag, "Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define APP_ASSERT(x, ...) { if(!(x)) { APP_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define APP_ASSERT_TAG(x, tag, ...) { if(!(x)) { APP_ERROR_TAG(tag, "Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define CORE_ASSERT(x, ...)
#define CORE_ASSERT_TAG(x, tag, ...)
#define APP_ASSERT(x, ...)
#define APP_ASSERT_TAG(x, tag, ...)
#endif