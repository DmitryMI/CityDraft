#pragma once

#include <QString>
#include "spdlog/spdlog.h"

namespace CityDraft::Logging
{

	class LogManager
	{
	public:
		static void InitLogging(const QString& logLevel);
		static std::shared_ptr<spdlog::logger> CreateLogger(const QString& name);
	};

}
