#include "chromapch.h"
#include "Chroma/Core/Input.h"

#include "Chroma/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Chroma
{
	Input::Gamepad Input::PrimaryGamepad = Gamepad::_1;
	std::function<void(Input::Joystick)> Input::GamepadConnectionCallback;
	std::function<void(Input::Joystick)> Input::GamepadDisconnectionCallback;

	void Input::Init()
	{
		glfwSetJoystickCallback(JoystickCallback);
	}

	bool Input::IsKeyPressed(Key keycode)
	{
		auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(Mouse button)
	{
		auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int>(button));
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return std::pair<float, float>(static_cast<float>(x), static_cast<float>(y));
	}


	float Input::GetMouseX()
	{
		return GetMousePos().first;
	}

	float Input::GetMouseY()
	{
		return GetMousePos().second;
	}

	bool Input::IsJoystickConnected(Joystick gamepad)
	{
		return glfwJoystickPresent(static_cast<int>(gamepad));
	}

	bool Input::IsGamepad(Joystick gamepad)
	{
		return glfwJoystickIsGamepad(static_cast<int>(gamepad));
	}

	Input::Gamepad Input::GetFirstConnectedGamepad()
	{
		for (int i = 0; i < static_cast<int>(Input::Gamepad::_LAST); i++)
		{
			if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
				return static_cast<Input::Gamepad>(i);
		}
		return Gamepad::_NULL;
	}

	Input::Joystick Input::GetFirstConnectedJoystick()
	{
		for (int i = 0; i < static_cast<int>(Input::Joystick::_LAST); i++)
		{
			if (glfwJoystickPresent(i))
				return static_cast<Input::Gamepad>(i);
		}
		return Gamepad::_NULL;
	}

	std::vector<Input::Joystick> Input::GetAllConnectedControllers()
	{
		std::vector<Joystick> joystick;
		for (int i = 0; i < static_cast<int>(Input::Joystick::_LAST); i++)
		{
			if (glfwJoystickPresent(i))
				joystick.push_back(static_cast<Input::Joystick>(i));
		}
		return joystick;
	}

	const Input::JoystickHatState *Input::GetJoystickHats(Joystick gamepad, int *count)
	{
		return (JoystickHatState *)glfwGetJoystickHats(static_cast<int>(gamepad), count);
	}

	Input::Gamepad Input::SetPrimaryGamepad()
	{
		for (int i = 0; i < static_cast<int>(Input::Joystick::_LAST); i++)
		{
			if (glfwJoystickPresent(i) == GLFW_TRUE)
			{
				return PrimaryGamepad = static_cast<Input::Joystick>(i);
			}
		}
		return Joystick::_NULL;
	}

	std::vector<float> Input::GetGamepadAxis(Gamepad gamepad)
	{
		std::vector<float> ret;
		int count;
		const float *axes = glfwGetJoystickAxes(static_cast<int>(gamepad), &count);
		for (int i = 0; i < count; i++)
		{
			ret.push_back(axes[count]);
		}
		return ret;
	}

	Input::ButtonState Input::GetGamepadButtonState(GamepadButton button, Gamepad gamepad)
	{
		return GetJoystickButtonState(button, gamepad);
	}

	Input::ButtonState Input::GetJoystickButtonState(JoystickButton button, Joystick gamepad)
	{
		if (glfwJoystickPresent(static_cast<int>(gamepad)) != GLFW_TRUE)
			return ButtonState::RELEASED;
		int count;
		const unsigned char *buttons = glfwGetJoystickButtons(static_cast<int>(gamepad), &count);
		if (buttons == nullptr)
			return ButtonState::RELEASED;
		if (buttons[static_cast<int>(button) - 1] == GLFW_TRUE)
			return ButtonState::PRESSED;
		return ButtonState::RELEASED;
	}


	Input::GamepadState Input::GetGamepadState(Gamepad gamepad)
	{
		GLFWgamepadstate glfwstate;
		int connected = glfwGetGamepadState(static_cast<int>(gamepad), &glfwstate);
		GamepadState state(glfwstate.buttons, glfwstate.axes);
		return state;
	}

	Input::ConnectionState Input::GetGamepadConnectionState(Gamepad gamepad)
	{
		return static_cast<ConnectionState>(glfwJoystickPresent((int)gamepad));
	}

	const char *Input::GetGamepadName(Gamepad gamepad)
	{
		return glfwGetGamepadName(static_cast<int>(gamepad));
	}

	void Input::JoystickCallback(int jid, int evnt)
	{
		if (evnt == GLFW_CONNECTED && GamepadConnectionCallback)
		{
			GamepadConnectionCallback(static_cast<Joystick>(jid));
		}
		else if (evnt == GLFW_DISCONNECTED && GamepadDisconnectionCallback)
		{
			GamepadDisconnectionCallback(static_cast<Joystick>(jid));
		}
	}

	void Input::SetGamepadConnectionCallback(std::function<void(Joystick)> func)
	{
		GamepadConnectionCallback = func;
	}

	void Input::SetGamepadDisconnectionCallback(std::function<void(Joystick)> func)
	{
		GamepadDisconnectionCallback = func;
	}
}
