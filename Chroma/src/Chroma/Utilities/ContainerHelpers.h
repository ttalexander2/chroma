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

	template<typename ContainerType>
	void RemoveAtIndex(ContainerType& container, size_t index)
	{
		if ((index) >= container.size())
			return;
		size_t i = index;
		while (i + 1 < container.size())
		{
			std::swap(container[i], container[i + 1]);
			i++;
		}

		container.pop_back();
	}

	template <typename T>
	T* PopValue(std::vector<T>& container, T& val)
	{
		if (container.empty())
			return nullptr;
		bool swapping = false;
		size_t i = 0;
		while (i < container.size())
		{
			if (container[i] == val)
				swapping = true;
			if (swapping && i < container.size() - 1)
				std::swap(container[i], container[i + 1]);
			i++;
		}
		T* retval = nullptr;
		if (swapping)
		{
			retval = &container.back();
			container.pop_back();
		}
		return retval;
	}
}