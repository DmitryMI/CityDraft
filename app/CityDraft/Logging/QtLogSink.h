#pragma once

#include <QDebug>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/base_sink.h"
#include "spdlog/fmt/fmt.h"


template<typename Mutex>
class QtLogSink : public spdlog::sinks::base_sink<Mutex>
{
public:
	std::vector<std::string> stored_messages;

protected:
	virtual void sink_it_(const spdlog::details::log_msg& msg) override
	{
		spdlog::memory_buf_t formatted;
		this->formatter_->format(msg, formatted);
		std::string log_str = fmt::to_string(formatted);

		switch (msg.level)
		{
		case spdlog::level::trace:
		case spdlog::level::debug:
			qDebug() << log_str;
			break;
		case spdlog::level::info:
			qInfo() << log_str;
			break;
		case spdlog::level::warn:
			qWarning() << log_str;
			break;
		case spdlog::level::err:
			qCritical() << log_str;
			break;
		case spdlog::level::critical:
			qFatal() << log_str;
			break;
		default:
			break;
		}
	}

	virtual void flush_() override
	{

	}
};
