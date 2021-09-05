#pragma once

#include <spdlog/spdlog.h>

namespace Polychrome
{
	class LogWindow
	{
	public:
		static bool Open;
		static void Init();
		static void Draw();

		enum class ErrorLevel
		{
			Trace,
			Info,
			Warning,
			Error,
			Critical
		};

		static const std::string ToString(ErrorLevel level);

		struct LogMessage
		{
			ErrorLevel Level;
			std::string Time;
			std::string Message;
			std::string Filename;
			std::string Function;
			int Line = 0;
		};

		static std::vector<LogMessage> Messages;
	};
}


