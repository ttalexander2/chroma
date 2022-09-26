#include "chromapch.h"
#include "WindowsWindow.h"

#include "Chroma/Events/ApplicationEvent.h"
#include "Chroma/Events/KeyEvent.h"
#include "Chroma/Events/MouseEvent.h"
#include "Chroma/Events/Event.h"
#include "Chroma/Platform/OpenGL/OpenGLContext.h"
#include "Chroma/Platform/Vulkan/VulkanContext.h"
#include "Chroma/Renderer/RendererAPI.h"

#include <glad/glad.h>

namespace Chroma
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char *description)
	{
		CHROMA_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window *Window::Create(const WindowProps &props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps &props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps &props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
			int success = glfwInit();
			CHROMA_CORE_ASSERT(success, "Count not initialize GLFW.");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if (props.Decorated)
		{
			glfwWindowHint(GLFW_DECORATED, true);
		}
		else
		{
			glfwWindowHint(GLFW_DECORATED, false);
		}

		m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);

		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			m_Context = new OpenGLContext(m_Window);
		else if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			m_Context = new VulkanContext(m_Window);
		else
			throw std::runtime_error("Other rendering APIs are not yet supported!");

		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		//SetVSync(false);

		//Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window,
				[](GLFWwindow *window, int width, int height)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
					data.Width = width;
					data.Height = height;

					WindowResizeEvent event(width, height);
					//CHROMA_CORE_WARN("{0}, {1}", width, height);
					data.EventCallback(event);
				});

		glfwSetWindowCloseCallback(m_Window,
				[](GLFWwindow *window)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

					WindowCloseEvent event;
					data.EventCallback(event);
				});

		glfwSetKeyCallback(m_Window,
				[](GLFWwindow *window, int key, int scancode, int action, int mods)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

					switch (action)
					{
						case GLFW_PRESS:
						{
							KeyPressedEvent event(key, 0);
							data.EventCallback(event);
							break;
						}
						case GLFW_RELEASE:
						{
							KeyReleasedEvent event(key);
							break;
						}
						case GLFW_REPEAT:
						{
							KeyPressedEvent event(key, 1);
							data.EventCallback(event);
							break;
						}
					}
				});

		glfwSetMouseButtonCallback(m_Window,
				[](GLFWwindow *window, int button, int action, int mods)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

					switch (action)
					{
						case GLFW_PRESS:
						{
							MouseButtonPressedEvent event(button);
							data.EventCallback(event);
							break;
						}
						case GLFW_RELEASE:
						{
							MouseButtonReleasedEvent event(button);
							data.EventCallback(event);
							break;
						}
					}
				});

		glfwSetCharCallback(m_Window,
				[](GLFWwindow *window, unsigned int keycode)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

					KeyTypedEvent event(keycode);
					data.EventCallback(event);
				});

		glfwSetScrollCallback(m_Window,
				[](GLFWwindow *window, double xOffset, double yOffset)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

					MouseScrolledEvent event((xOffset), (yOffset));
					data.EventCallback(event);
				});

		glfwSetCursorPosCallback(m_Window,
				[](GLFWwindow *window, double xPos, double yPos)
				{
					WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

					MouseMovedEvent event((xPos), (yPos));
					data.EventCallback(event);
				});
	}

	void WindowsWindow::Shutdown()
	{
		delete m_Context;
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}
