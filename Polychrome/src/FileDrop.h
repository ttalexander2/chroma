#pragma once
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"

namespace Polychrome
{
	class FileDrop
	{
	public:
#ifdef CHROMA_PLATFORM_WINDOWS
		static void HandleFileDrop(GLFWwindow* window, int count, const char** paths);
#endif
	};
}


