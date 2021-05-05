#include "FileDrop.h"
#include <string>
#include "Chroma.h"

namespace Polychrome
{
#ifdef CHROMA_PLATFORM_WINDOWS
	void FileDrop::HandleFileDrop(GLFWwindow* window, int count, const char** paths)
	{
		for (int i = 0; i < count; i++)
		{
			std::string path = paths[i];
			//Do file drop shit
			CHROMA_ERROR("FILE_DROP: {}", path);
			
		}
	}
#endif
}

