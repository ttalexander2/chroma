#pragma once
#include <string>

namespace Chroma
{
	class Clipboard
	{
	public:
		static void Write(const std::string &text);
		static std::string Read();
	};
}
