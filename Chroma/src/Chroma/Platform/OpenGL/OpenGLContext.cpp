#include "chromapch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Chroma
{
	OpenGLContext::OpenGLContext(GLFWwindow *windowHandle) :
		m_WindowHandle(windowHandle)
	{
		CHROMA_CORE_ASSERT(windowHandle, "Window Handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CHROMA_CORE_ASSERT(status, "Failed to load GLAD");

		CHROMA_CORE_INFO("OpenGL version {0}", glGetString(GL_VERSION));
		CHROMA_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		CHROMA_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
