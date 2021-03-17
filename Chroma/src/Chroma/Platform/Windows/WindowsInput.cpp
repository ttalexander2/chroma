#include "chromapch.h"
#include "Chroma/Core/Input.h"

#include "Chroma/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Chroma
{

	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return std::pair<float, float>((double)x, (double)y);
	}

	float Input::GetMouseX()
	{
		return GetMousePos().first;
	}

	float Input::GetMouseY()
	{
		return GetMousePos().second;
	}
}
