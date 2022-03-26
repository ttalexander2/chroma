#pragma once

#include <string>

namespace Chroma
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter, void* nativeHandle = nullptr);
		static std::string SaveFile(const char* filter, void* nativeHandle = nullptr);
		static std::string OpenDirectory(void* nativeHandle = nullptr);
	};
}