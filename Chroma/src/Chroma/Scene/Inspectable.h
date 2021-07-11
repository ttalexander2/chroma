#pragma once
#include <string>

namespace Chroma
{
	class Inspectable
	{
	public:
		virtual const std::string ClassName() const = 0;
	};
}