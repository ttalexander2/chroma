#include "chromapch.h"
#include "Chroma/Utilities/Clipboard.h"

#include "Chroma/Core/Application.h"
#include "Chroma/Core/Window.h"
#include "GLFW/glfw3.h"


namespace Chroma
{
	void Clipboard::Write(const std::string &text)
	{
		glfwSetClipboardString(static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow()), text.c_str());
	}

	std::string Clipboard::Read()
	{
		const char *text = glfwGetClipboardString(static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow()));
		return text;
	}
}
