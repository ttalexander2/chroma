#include "chromapch.h"
#include "Chroma/Core/Input.h"

#include "Chroma/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Chroma
{


	Input::Gamepad Input::PrimaryGamepad = Input::Gamepad::_1;
	std::function<void(Input::Joystick)> Input::GamepadConnectionCallback;
	std::function<void(Input::Joystick)> Input::GamepadDisconnectionCallback;

	void Input::Init()
	{
		glfwSetJoystickCallback(JoystickCallback);
	}

	bool Input::IsKeyPressed(Input::Key keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, (int)keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(Input::Mouse button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, (int)button);
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

	bool Input::IsJoystickConnected(Input::Joystick gamepad)
	{
		return glfwJoystickPresent((int)gamepad);
	}
	bool Input::IsGamepad(Input::Joystick gamepad)
	{
		return glfwJoystickIsGamepad((int)gamepad);
	}
	Input::Gamepad Input::GetFirstConnectedGamepad()
	{
		for (int i = 0; i < (int)Input::Gamepad::_LAST; i++)
		{
			if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
				return (Input::Gamepad)i;
		}
		return Input::Gamepad::_NULL;
	}
	Input::Joystick Input::GetFirstConnectedJoystick()
	{
		for (int i = 0; i < (int)Input::Joystick::_LAST; i++)
		{
			if (glfwJoystickPresent(i))
				return (Input::Gamepad)i;
		}
		return Input::Gamepad::_NULL;
	}
	std::vector<Input::Joystick> Input::GetAllConnectedControllers()
	{
		std::vector<Joystick> joystick;
		for (int i = 0; i < (int)Input::Joystick::_LAST; i++)
		{
			if (glfwJoystickPresent(i))
				joystick.push_back((Input::Joystick)i);
		}
		return joystick;
	}

	const Input::JoystickHatState* Input::GetJoystickHats(Joystick gamepad, int* count)
	{
		return (JoystickHatState*)glfwGetJoystickHats((int)gamepad, count);
	}

	Input::Gamepad Input::SetPrimaryGamepad()
	{
		for (int i = 0; i < (int)Input::Joystick::_LAST; i++)
		{
			if (glfwJoystickPresent(i) == GLFW_TRUE)
			{
				return PrimaryGamepad = (Input::Joystick)i;
			}
		}
		return Joystick::_NULL;
	}

	std::vector<float> Input::GetGamepadAxis(Input::Gamepad gamepad)
	{
		std::vector<float> ret;
		int count;
		const float* axes = glfwGetJoystickAxes((int)gamepad, &count);
		for (int i = 0; i < count; i++)
		{
			ret.push_back(axes[count]);
		}
		return ret;
	}

	Input::ButtonState Input::GetGamepadButtonState(Input::GamepadButton button, Input::Gamepad gamepad)
	{
		return GetJoystickButtonState(button, gamepad);
	}

	Input::ButtonState Input::GetJoystickButtonState(Input::JoystickButton button, Input::Joystick gamepad)
	{
		if (glfwJoystickPresent((int)gamepad) != GLFW_TRUE)
			return ButtonState::RELEASED;
		int count;
		const unsigned char* buttons = glfwGetJoystickButtons((int)gamepad, &count);
		if (buttons == nullptr)
			return ButtonState::RELEASED;
		if (buttons[(int)button - 1] == GLFW_TRUE)
			return ButtonState::PRESSED;
		return ButtonState::RELEASED;
	}


	Input::GamepadState Input::GetGamepadState(Gamepad gamepad)
	{
		GLFWgamepadstate glfwstate;
		int connected = glfwGetGamepadState((int)gamepad, &glfwstate);
		GamepadState state(glfwstate.buttons, glfwstate.axes);
		return state;
	}
	Input::ConnectionState Input::GetGamepadConnectionState(Gamepad gamepad)
	{
		return (ConnectionState)glfwJoystickPresent((int)gamepad);
	}

	const char* Input::GetGamepadName(Input::Gamepad gamepad)
	{
		return glfwGetGamepadName((int)gamepad);
	}

	void Input::JoystickCallback(int jid, int evnt)
	{
		if (evnt == GLFW_CONNECTED && Input::GamepadConnectionCallback)
		{
			Input::GamepadConnectionCallback((Joystick)jid);
		}
		else if (evnt == GLFW_DISCONNECTED && Input::GamepadDisconnectionCallback)
		{
			Input::GamepadDisconnectionCallback((Joystick)jid);
		}
	}

	void Input::SetGamepadConnectionCallback(std::function<void(Joystick)> func)
	{
		Input::GamepadConnectionCallback = func;
	}

	void Input::SetGamepadDisconnectionCallback(std::function<void(Joystick)> func)
	{
		Input::GamepadDisconnectionCallback = func;
	}



}
