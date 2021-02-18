#include "chromapch.h"
#include "WindowsInput.h"

#include "Chroma/Application.h"
#include <GLFW/glfw3.h>

namespace Chroma
{

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return std::pair<float, float>((double)x, (double)y);
	}

	float WindowsInput::GetMouseXImpl()
	{
		return GetMousePosImpl().first;
	}

	float WindowsInput::GetMouseYImpl()
	{
		return GetMousePosImpl().second;
	}
}
