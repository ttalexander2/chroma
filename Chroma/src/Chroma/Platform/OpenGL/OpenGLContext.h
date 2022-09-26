#pragma once

#include "Chroma/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Chroma
{
	/// @brief Class containing the OpenGL Context (window to render to).
	class OpenGLContext : public GraphicsContext
	{
	public:
		/// @brief Constructs an OpenGLContext object given a GLFW window.
		/// @param windowHandle Window Context.
		OpenGLContext(GLFWwindow *windowHandle);

		/// @brief Initialized the context.
		void Init() override;

		/// @brief Swaps the front and back buffers.
		void SwapBuffers() override;

	private:
		/// @brief Window handle.
		GLFWwindow *m_WindowHandle;
	};
}
