#pragma once

#include <string>
#include "spdlog/spdlog.h"

namespace CityDraft::Logging
{

	class LogManager
	{
	public:
		static void InitLogging(const std::string& logLevel);
		static std::shared_ptr<spdlog::logger> CreateLogger(const std::string& name);

	private:
		static bool m_LoggingInitialized;
	};

}
