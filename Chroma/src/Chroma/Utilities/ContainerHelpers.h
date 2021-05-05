#pragma once

namespace Chroma
{

	template<typename ContainerType>
	void SwapAndPopAtIndex(ContainerType& container, size_t index)
	{
		if ((index + 1) >= container.size())
			return;
		std::swap(container[index], container.back());
		container.pop_back();
	}
}