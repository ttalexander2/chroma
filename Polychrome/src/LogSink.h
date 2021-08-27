#pragma once
#include "spdlog/sinks/base_sink.h"
#include "LogWindow.h"
#include <spdlog/details/null_mutex.h>

namespace Polychrome
{

	template<typename Mutex>
	class LogSink : public spdlog::sinks::base_sink<Mutex>
	{
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{

			LogWindow::LogMessage message;
			if (msg.level == spdlog::level::level_enum::trace)
				message.Level = LogWindow::ErrorLevel::Trace;
			else if (msg.level == spdlog::level::level_enum::info)
				message.Level = LogWindow::ErrorLevel::Info;
			else if (msg.level == spdlog::level::level_enum::warn)
				message.Level = LogWindow::ErrorLevel::Warning;
			else if (msg.level == spdlog::level::level_enum::err)
				message.Level = LogWindow::ErrorLevel::Error;
			else if (msg.level == spdlog::level::level_enum::critical)
				message.Level = LogWindow::ErrorLevel::Critical;
			else
				message.Level = LogWindow::ErrorLevel::Trace;
			message.Message = std::string(msg.payload.data());
			std::time_t t = std::chrono::system_clock::to_time_t(msg.time);
			auto lt = std::localtime(&t);
			message.Time = fmt::format("{:00}:{:00}:{:00}", lt->tm_hour, lt->tm_min, lt->tm_sec);
			if (msg.source.filename)
				message.Filename = std::string(msg.source.filename);
			if (msg.source.funcname)
				message.Function = std::string(msg.source.funcname);
			if (msg.source.line)
				message.Line = msg.source.line;
			LogWindow::Messages.push_back(message);
		}

		void flush_() override
		{

		}
	};
}