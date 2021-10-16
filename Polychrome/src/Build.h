#pragma once
#include <string>
#include <vector>

namespace Polychrome
{
	class Build
	{
	public:

		struct BuildMessage
		{
			enum class Severity
			{
				Message = 0,
				Warning = 1,
				Error = 2
			};

			std::string file;
			int line = 0;
			int offset = 0;
			std::string error;
			Severity severity;
			std::string message;

			std::string source;
		};

		static void ParseErrors(std::ifstream& stream);
		static bool BuildMonoAssembly(const std::string& path, const std::string& name);

		static std::vector<BuildMessage> BuildMessages;
		static size_t Errors;
		static size_t Warnings;
		static size_t Messages;
		
		static bool Success();
	};
}