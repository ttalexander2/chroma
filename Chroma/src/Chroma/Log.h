#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"


namespace Chroma
{
	class CHROMA_API Log
	{
	public:
		static void Init();
		
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;  }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}


//Engine Log Macros
#define CHROMA_CORE_TRACE(...)     ::Chroma::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CHROMA_CORE_INFO(...)      ::Chroma::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CHROMA_CORE_WARN(...)      ::Chroma::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CHROMA_CORE_ERROR(...)     ::Chroma::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CHROMA_CORE_FATAL(...)     ::Chroma::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client Log Macros
#define CHROMA_TRACE(...)          ::Chroma::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CHROMA_INFO(...)           ::Chroma::Log::GetClientLogger()->info(__VA_ARGS__)
#define CHROMA_WARN(...)           ::Chroma::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CHROMA_ERROR(...)          ::Chroma::Log::GetClientLogger()->error(__VA_ARGS__)
#define CHROMA_FATAL(...)          ::Chroma::Log::GetClientLogger()->fatal(__VA_ARGS__)


