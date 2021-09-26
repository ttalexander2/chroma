#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>


namespace Chroma
{
	/// @brief Logging class, using the spdlog interface.
	///
	/// The logging api provides multiple macros for logging at different levels.
	/// Levels occur in the following order: Trace, Info, Warning, Error, and Fatal, from least to most significant.
	/// 
	/// spdlog can be configured in such a way to only provide warnings at a certain level.
	class CHROMA_API Log
	{
	public:
		/// @brief Initialize the logging system.
		static void Init();
		
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;  }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		enum class LogLevel
		{
			Trace = (1 << 0),
			Debug = (1 << 1),
			Info = (1 << 2),
			Warn = (1 << 3),
			Error = (1 << 4),
			Critical = (1 << 5)
		};

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}


/// Engine Log Macros

/// @brief CHROMA_CORE_TRACE
/// Logs the provided string at trace level.
#define CHROMA_CORE_TRACE(...)     ::Chroma::Log::GetCoreLogger()->trace(__VA_ARGS__)
/// @brief CHROMA_CORE_INFO
/// Logs the provided string at trace level.
#define CHROMA_CORE_INFO(...)      ::Chroma::Log::GetCoreLogger()->info(__VA_ARGS__)
/// @brief CHROMA_CORE_WARN
/// Logs the provided string at info level.
#define CHROMA_CORE_WARN(...)      ::Chroma::Log::GetCoreLogger()->warn(__VA_ARGS__)
/// @brief CHROMA_CORE_ERROR
/// Logs the provided string at warning level.
#define CHROMA_CORE_ERROR(...)     ::Chroma::Log::GetCoreLogger()->error(__VA_ARGS__)
/// @brief CHROMA_CORE_FATAL
/// Logs the provided string at fatal level.
#define CHROMA_CORE_CRITICAL(...)     ::Chroma::Log::GetCoreLogger()->critical(__VA_ARGS__)

/// Client Log Macros

#ifndef CHROMA_DEBUG_LOG

/// @brief CHROMA_TRACE
/// Logs the provided string at trace level.
#define CHROMA_TRACE(...)          ::Chroma::Log::GetClientLogger()->trace(__VA_ARGS__)
/// @brief CHROMA_INFO
/// Logs the provided string at info level.
#define CHROMA_INFO(...)           ::Chroma::Log::GetClientLogger()->info(__VA_ARGS__)
/// @brief CHROMA_WARN
/// Logs the provided string at warn level.
#define CHROMA_WARN(...)           ::Chroma::Log::GetClientLogger()->warn(__VA_ARGS__)
/// @brief CHROMA_ERROR
/// Logs the provided string at error level.
#define CHROMA_ERROR(...)          ::Chroma::Log::GetClientLogger()->error(__VA_ARGS__)
/// @brief CHROMA_FATAL
/// Logs the provided string at fatal level.
#define CHROMA_CRITICAL(...)          ::Chroma::Log::GetClientLogger()->critical(__VA_ARGS__)


#else

/// @brief CHROMA_TRACE
/// Logs the provided string at trace level.
#define CHROMA_TRACE(...)          ::Chroma::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::trace, __VA_ARGS__)
/// @brief CHROMA_INFO
/// Logs the provided string at info level.
#define CHROMA_INFO(...)           ::Chroma::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::info, __VA_ARGS__)
/// @brief CHROMA_WARN
/// Logs the provided string at warn level.
#define CHROMA_WARN(...)           ::Chroma::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::warn, __VA_ARGS__)
/// @brief CHROMA_ERROR
/// Logs the provided string at error level.
#define CHROMA_ERROR(...)          ::Chroma::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::err, __VA_ARGS__)
/// @brief CHROMA_FATAL
/// Logs the provided string at fatal level.
#define CHROMA_FATAL(...)          ::Chroma::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::critical, __VA_ARGS__)

#endif