#pragma once

#include "Reflection.h"

namespace Chroma
{

	namespace Reflection
	{
			template <typename T>
        	class ComponentInitializer
        	{
        	public:
        		ComponentInitializer()
        		{
        			Initializer::AddComponentInitializer<T>();
        		}
        	};
		
	}

}

