#pragma once

namespace Chroma
{
	template <typename T> 
	struct Data
	{
		operator->() { return value; }
	private:
		T value;
	};

}