#include "Log.h"


namespace ST
{
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_appLogger;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T][%n][%l]%v%$");
		s_coreLogger = spdlog::stdout_color_mt("Core");
		s_coreLogger->set_level(spdlog::level::trace);

		s_appLogger = spdlog::stdout_color_mt("App");
		s_appLogger->set_level(spdlog::level::trace);
	}

	void Log::destroy()
	{
		s_appLogger.reset();
		s_coreLogger.reset();
		spdlog::drop_all();
	}
}
