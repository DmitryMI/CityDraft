#include "LogManager.h"
#include "spdlog/spdlog.h"
#include <mutex>
#include <string>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace CityDraft::Logging
{
	bool LogManager::m_LoggingInitialized = false;

	void LogManager::InitLogging(const std::string& logLevel)
	{
		spdlog::level::level_enum level = spdlog::level::from_str(logLevel);
		spdlog::set_level(level);
		spdlog::info("Logging level set to {}", spdlog::level::to_string_view(spdlog::get_level()));

		auto loggerDefault = CreateLogger("general");
		spdlog::set_default_logger(loggerDefault);
		m_LoggingInitialized = true;
	}

	std::shared_ptr<spdlog::logger> LogManager::CreateLogger(const std::string& name)
	{
		if (!m_LoggingInitialized)
		{
			InitLogging("debug");
		}

		std::shared_ptr<spdlog::logger> logger = spdlog::get(name);
		if (logger)
		{
			return logger;
		}

		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

		spdlog::logger loggerNew(name);
		loggerNew.sinks().push_back(consoleSink);
		loggerNew.set_level(spdlog::get_level());
		loggerNew.set_pattern("[%Y-%m-%d %T.%e] [%-10n] [%^%-8l%$] %v");
		return std::make_shared<spdlog::logger>(loggerNew);
	}

}
