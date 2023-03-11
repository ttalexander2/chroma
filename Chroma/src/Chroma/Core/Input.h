#pragma once

#include "Chroma/Core/Core.h"

namespace Chroma
{
	/// @brief Input class for polling mouse and keyboard input.
	class Input
	{
	public:
		//For lua bindings
		Input() = default;

		static void Init();

#pragma region Mouse & Keyboard


		enum class Mouse
		{
			BUTTON_1 = 0,
			BUTTON_2 = 1,
			BUTTON_3 = 2,
			BUTTON_4 = 3,
			BUTTON_5 = 4,
			BUTTON_6 = 5,
			BUTTON_7 = 6,
			BUTTON_8 = 7,
			_LAST = BUTTON_8,
			LEFT = BUTTON_1,
			RIGHT = BUTTON_2,
			MIDDLE = BUTTON_3
		};

		//Adapted from glfw3.h
		enum class Key
		{
			/// PRINTABLE KEYS
			SPACE = 32,
			APOSTROPHE = 39,
			/* ' */
			COMMA = 44,
			/* , */
			MINUS = 45,
			/* - */
			PERIOD = 46,
			/* . */
			SLASH = 47,
			/* / */
			K_0 = 48,
			K_1 = 49,
			K_2 = 50,
			K_3 = 51,
			K_4 = 52,
			K_5 = 53,
			K_6 = 54,
			K_7 = 55,
			K_8 = 56,
			K_9 = 57,
			SEMICOLON = 59,
			/* ; */
			EQUAL = 61,
			/* = */
			A = 65,
			B = 66,
			C = 67,
			D = 68,
			E = 69,
			F = 70,
			G = 71,
			H = 72,
			I = 73,
			J = 74,
			K = 75,
			L = 76,
			M = 77,
			N = 78,
			O = 79,
			P = 80,
			Q = 81,
			R = 82,
			S = 83,
			T = 84,
			U = 85,
			V = 86,
			W = 87,
			X = 88,
			Y = 89,
			Z = 90,
			LEFT_BRACKET = 91,
			/* [ */
			BACKSLASH = 92,
			/* \ */
			RIGHT_BRACKET = 93,
			/* ] */
			GRAVE_ACCENT = 96,
			/* ` */
			WORLD_1 = 161,
			/* non-US #1 */
			WORLD_2 = 162,
			/* non-US #2 */

			ESCAPE = 256,
			ENTER = 257,
			TAB = 258,
			BACKSPACE = 259,
			INSERT = 260,
			DEL = 261,
			RIGHT = 262,
			LEFT = 263,
			DOWN = 264,
			UP = 265,
			PAGE_UP = 266,
			PAGE_DOWN = 267,
			HOME = 268,
			END = 269,
			CAPS_LOCK = 280,
			SCROLL_LOCK = 281,
			NUM_LOCK = 282,
			PRINT_SCREEN = 283,
			PAUSE = 284,
			F1 = 290,
			F2 = 291,
			F3 = 292,
			F4 = 293,
			F5 = 294,
			F6 = 295,
			F7 = 296,
			F8 = 297,
			F9 = 298,
			F10 = 299,
			F11 = 300,
			F12 = 301,
			F13 = 302,
			F14 = 303,
			F15 = 304,
			F16 = 305,
			F17 = 306,
			F18 = 307,
			F19 = 308,
			F20 = 309,
			F21 = 310,
			F22 = 311,
			F23 = 312,
			F24 = 313,
			F25 = 314,
			KP_0 = 320,
			KP_1 = 321,
			KP_2 = 322,
			KP_3 = 323,
			KP_4 = 324,
			KP_5 = 325,
			KP_6 = 326,
			KP_7 = 327,
			KP_8 = 328,
			KP_9 = 329,
			KP_DECIMAL = 330,
			KP_DIVIDE = 331,
			KP_MULTIPLY = 332,
			KP_SUBTRACT = 333,
			KP_ADD = 334,
			KP_ENTER = 335,
			KP_EQUAL = 336,
			LEFT_SHIFT = 340,
			LEFT_CONTROL = 341,
			LEFT_ALT = 342,
			LEFT_SUPER = 343,
			RIGHT_SHIFT = 344,
			RIGHT_CONTROL = 345,
			RIGHT_ALT = 346,
			RIGHT_SUPER = 347,
			MENU = 348,

			_LAST = MENU
		};

		/// @brief Check if a specific key is pressed.
		static bool IsKeyPressed(Key keycode);
		/// @brief Check if a mouse button is pressed.
		static bool IsMouseButtonPressed(Mouse button);
		/// @brief Get the current mouse position.
		static std::pair<float, float> GetMousePos();
		/// @brief Get the mouse X position.
		static float GetMouseX();
		/// @brief Get the mouse Y position.
		static float GetMouseY();


#pragma endregion

#pragma region Gamepad


		/// @brief Enumeration for All Joysticks/Gamepads
		enum class Joystick
		{
			_1 = 0,
			_2 = 1,
			_3 = 2,
			_4 = 3,
			_5 = 4,
			_6 = 5,
			_7 = 6,
			_8 = 7,
			_9 = 8,
			_10 = 9,
			_11 = 10,
			_12 = 11,
			_13 = 12,
			_14 = 13,
			_15 = 14,
			_16 = 15,
			_LAST = _16,
			_NULL = 0
		};

		/// @brief Gamepad alias for Joystick
		using Gamepad = Joystick;

		/// @brief Primary Gamepad
		static Gamepad PrimaryGamepad;

		/// @brief Button State for Gamepad/Joystick
		enum class ButtonState
		{
			RELEASED = 0,
			PRESSED = 1
		};

		/// @brief Possible Gamepad Buttons
		enum class GamepadButton
		{
			A = 0,
			B = 1,
			X = 2,
			Y = 3,
			LEFT_BUMPER = 4,
			RIGHT_BUMPER = 5,
			BACK = 6,
			START = 7,
			GUIDE = 8,
			LEFT_THUMB = 9,
			RIGHT_THUMB = 10,
			DPAD_UP = 11,
			DPAD_RIGHT = 12,
			DPAD_DOWN = 13,
			DPAD_LEFT = 14,
			_LAST = DPAD_DOWN,
			CROSS = A,
			CIRCLE = B,
			SQUARE = X,
			TRIANGLE = Y
		};

		enum class GamepadAxis
		{
			LEFT_X = 0,
			LEFT_Y = 1,
			RIGHT_X = 2,
			RIGHT_Y = 3,
			LEFT_TRIGGER = 4,
			RIGHT_TRIGGER = 5,
			_LAST = RIGHT_TRIGGER
		};

		using JoystickButton = GamepadButton;

		/// @brief Struct representing the state of a gamepad
		struct GamepadState
		{
			/// @brief Number of total buttons
			static constexpr size_t ButtonCount = 15;
			/// @brief Number of pollable Axes
			static constexpr size_t AxisCount = 6;

			const ButtonState operator[](GamepadButton button) const
			{
				return Buttons[static_cast<int>(button)];
			}

			const float operator[](GamepadAxis button) const
			{
				return Axis[static_cast<int>(button)];
			}

			/// @brief Gets the state of a specific button
			/// @param button Button to check state
			/// @return Button state
			const ButtonState GetButtonState(GamepadButton button) const
			{
				return Buttons[static_cast<int>(button)];
			}

			/// @brief Gets an array of size ButtonCount. Indexable with GamepadButton enum.
			/// @return  Array containing the states of all the buttons.
			const ButtonState *GetButtonStates() const
			{
				return Buttons;
			}

			const float GetAxisState(GamepadAxis axis) const
			{
				return Axis[static_cast<int>(axis)];
			}

			const float *GetAxisStates() const
			{
				return Axis;
			}

		private:
			GamepadState(unsigned char buttons[ButtonCount], float axis[AxisCount])
			{
				for (int i = 0; i < ButtonCount; i++)
				{
					Buttons[i] = static_cast<ButtonState>(buttons[i]);
				}
				for (int i = 0; i < AxisCount; i++)
				{
					Axis[i] = axis[i];
				}
			}

			ButtonState Buttons[ButtonCount];
			float Axis[AxisCount];

			friend class Input;
		};

		enum class ConnectionState
		{
			DISCONNECTED = 0,
			CONNECTED = 1
		};

		enum class JoystickHatState
		{
			CENTERED = 0,
			UP = 1,
			RIGHT = 2,
			DOWN = 4,
			LEFT = 8,
			RIGHT_UP = RIGHT | UP,
			RIGHT_DOWN = RIGHT | DOWN,
			LEFT_UP = LEFT | UP,
			LEFT_DOWN = LEFT | DOWN
		};


		static bool IsJoystickConnected(Joystick gamepad = PrimaryGamepad);
		static bool IsGamepad(Joystick gamepad = PrimaryGamepad);
		static Gamepad GetFirstConnectedGamepad();
		static Joystick GetFirstConnectedJoystick();
		static std::vector<Joystick> GetAllConnectedControllers();
		static const JoystickHatState *GetJoystickHats(Joystick gamepad, int *count);

		static Gamepad SetPrimaryGamepad();

		static std::vector<float> GetGamepadAxis(Gamepad gamepad = PrimaryGamepad);
		static ButtonState GetGamepadButtonState(GamepadButton button, Gamepad gamepad = PrimaryGamepad);
		static ButtonState GetJoystickButtonState(JoystickButton button, Joystick gamepad = PrimaryGamepad);

		static GamepadState GetGamepadState(Gamepad gamepad = PrimaryGamepad);
		static ConnectionState GetGamepadConnectionState(Gamepad gamepad = PrimaryGamepad);

		static const char *GetGamepadName(Gamepad = PrimaryGamepad);

		static void SetGamepadConnectionCallback(std::function<void(Joystick)> func);
		static void SetGamepadDisconnectionCallback(std::function<void(Joystick)> func);


#pragma endregion

	private:
		static std::function<void(Joystick)> GamepadConnectionCallback;
		static std::function<void(Joystick)> GamepadDisconnectionCallback;

		static void JoystickCallback(int jit, int evnt);
	};
}

//MACROS

/// PRINTABLE KEYS
#define CHROMA_KEY_SPACE              Chroma::Input::Key::SPACE
#define CHROMA_KEY_APOSTROPHE         Chroma::Input::Key::APOSTROPHE  /* ' */
#define CHROMA_KEY_COMMA              Chroma::Input::Key::COMMA  /* , */
#define CHROMA_KEY_MINUS              Chroma::Input::Key::MINUS  /* - */
#define CHROMA_KEY_PERIOD             Chroma::Input::Key::PERIOD  /* . */
#define CHROMA_KEY_SLASH              Chroma::Input::Key::SLASH  /* / */
#define CHROMA_KEY_0                  Chroma::Input::Key::K_0
#define CHROMA_KEY_1                  Chroma::Input::Key::K_1
#define CHROMA_KEY_2                  Chroma::Input::Key::K_2
#define CHROMA_KEY_3                  Chroma::Input::Key::K_3
#define CHROMA_KEY_4                  Chroma::Input::Key::K_4
#define CHROMA_KEY_5                  Chroma::Input::Key::K_5
#define CHROMA_KEY_6                  Chroma::Input::Key::K_6
#define CHROMA_KEY_7                  Chroma::Input::Key::K_7
#define CHROMA_KEY_8                  Chroma::Input::Key::K_8
#define CHROMA_KEY_9                  Chroma::Input::Key::K_9
#define CHROMA_KEY_SEMICOLON          Chroma::Input::Key::SEMICOLON  /* ; */
#define CHROMA_KEY_EQUAL              Chroma::Input::Key::EQUAL  /* = */
#define CHROMA_KEY_A                  Chroma::Input::Key::A
#define CHROMA_KEY_B                  Chroma::Input::Key::B
#define CHROMA_KEY_C                  Chroma::Input::Key::C
#define CHROMA_KEY_D                  Chroma::Input::Key::D
#define CHROMA_KEY_E                  Chroma::Input::Key::E
#define CHROMA_KEY_F                  Chroma::Input::Key::F
#define CHROMA_KEY_G                  Chroma::Input::Key::G
#define CHROMA_KEY_H                  Chroma::Input::Key::H
#define CHROMA_KEY_I                  Chroma::Input::Key::I
#define CHROMA_KEY_J                  Chroma::Input::Key::J
#define CHROMA_KEY_K                  Chroma::Input::Key::K
#define CHROMA_KEY_L                  Chroma::Input::Key::L
#define CHROMA_KEY_M                  Chroma::Input::Key::M
#define CHROMA_KEY_N                  Chroma::Input::Key::N
#define CHROMA_KEY_O                  Chroma::Input::Key::O
#define CHROMA_KEY_P                  Chroma::Input::Key::P
#define CHROMA_KEY_Q                  Chroma::Input::Key::Q
#define CHROMA_KEY_R                  Chroma::Input::Key::R
#define CHROMA_KEY_S                  Chroma::Input::Key::S
#define CHROMA_KEY_T                  Chroma::Input::Key::T
#define CHROMA_KEY_U                  Chroma::Input::Key::U
#define CHROMA_KEY_V                  Chroma::Input::Key::V
#define CHROMA_KEY_W                  Chroma::Input::Key::W
#define CHROMA_KEY_X                  Chroma::Input::Key::X
#define CHROMA_KEY_Y                  Chroma::Input::Key::Y
#define CHROMA_KEY_Z                  Chroma::Input::Key::Z
#define CHROMA_KEY_LEFT_BRACKET       Chroma::Input::Key::LEFT_BRACKET  /* [ */
#define CHROMA_KEY_BACKSLASH          Chroma::Input::Key::BACKSLASH  /* \ */
#define CHROMA_KEY_RIGHT_BRACKET      Chroma::Input::Key::RIGHT_BRACKET  /* ] */
#define CHROMA_KEY_GRAVE_ACCENT       Chroma::Input::Key::GRAVE_ACCENT  /* ` */
#define CHROMA_KEY_WORLD_1            Chroma::Input::Key::WORLD_1 /* non-US #1 */
#define CHROMA_KEY_WORLD_2            Chroma::Input::Key::WORLD_2 /* non-US #2 */

/* Function keys */
#define CHROMA_KEY_ESCAPE             Chroma::Input::Key::ESCAPE
#define CHROMA_KEY_ENTER              Chroma::Input::Key::ENTER
#define CHROMA_KEY_TAB                Chroma::Input::Key::TAB
#define CHROMA_KEY_BACKSPACE          Chroma::Input::Key::BACKSPACE
#define CHROMA_KEY_INSERT             Chroma::Input::Key::INSERT
#define CHROMA_KEY_DELETE             Chroma::Input::Key::DEL
#define CHROMA_KEY_RIGHT              Chroma::Input::Key::RIGHT
#define CHROMA_KEY_LEFT               Chroma::Input::Key::LEFT
#define CHROMA_KEY_DOWN               Chroma::Input::Key::DOWN
#define CHROMA_KEY_UP                 Chroma::Input::Key::UP
#define CHROMA_KEY_PAGE_UP            Chroma::Input::Key::PAGE_UP
#define CHROMA_KEY_PAGE_DOWN          Chroma::Input::Key::PAGE_DOWN
#define CHROMA_KEY_HOME               Chroma::Input::Key::HOME
#define CHROMA_KEY_END                Chroma::Input::Key::END
#define CHROMA_KEY_CAPS_LOCK          Chroma::Input::Key::CAPS_LOCK
#define CHROMA_KEY_SCROLL_LOCK        Chroma::Input::Key::SCROLL_LOCK
#define CHROMA_KEY_NUM_LOCK           Chroma::Input::Key::NUM_LOCK
#define CHROMA_KEY_PRINT_SCREEN       Chroma::Input::Key::PRINT_SCREEN
#define CHROMA_KEY_PAUSE              Chroma::Input::Key::PAUSE
#define CHROMA_KEY_F1                 Chroma::Input::Key::F1
#define CHROMA_KEY_F2                 Chroma::Input::Key::F2
#define CHROMA_KEY_F3                 Chroma::Input::Key::F3
#define CHROMA_KEY_F4                 Chroma::Input::Key::F4
#define CHROMA_KEY_F5                 Chroma::Input::Key::F5
#define CHROMA_KEY_F6                 Chroma::Input::Key::F6
#define CHROMA_KEY_F7                 Chroma::Input::Key::F7
#define CHROMA_KEY_F8                 Chroma::Input::Key::F8
#define CHROMA_KEY_F9                 Chroma::Input::Key::F9
#define CHROMA_KEY_F10                Chroma::Input::Key::F10
#define CHROMA_KEY_F11                Chroma::Input::Key::F11
#define CHROMA_KEY_F12                Chroma::Input::Key::F12
#define CHROMA_KEY_F13                Chroma::Input::Key::F13
#define CHROMA_KEY_F14                Chroma::Input::Key::F14
#define CHROMA_KEY_F15                Chroma::Input::Key::F15
#define CHROMA_KEY_F16                Chroma::Input::Key::F16
#define CHROMA_KEY_F17                Chroma::Input::Key::F17
#define CHROMA_KEY_F18                Chroma::Input::Key::F18
#define CHROMA_KEY_F19                Chroma::Input::Key::F19
#define CHROMA_KEY_F20                Chroma::Input::Key::F20
#define CHROMA_KEY_F21                Chroma::Input::Key::F21
#define CHROMA_KEY_F22                Chroma::Input::Key::F22
#define CHROMA_KEY_F23                Chroma::Input::Key::F23
#define CHROMA_KEY_F24                Chroma::Input::Key::F24
#define CHROMA_KEY_F25                Chroma::Input::Key::F25
#define CHROMA_KEY_KP_0               Chroma::Input::Key::KP_0
#define CHROMA_KEY_KP_1               Chroma::Input::Key::KP_1
#define CHROMA_KEY_KP_2               Chroma::Input::Key::KP_2
#define CHROMA_KEY_KP_3               Chroma::Input::Key::KP_3
#define CHROMA_KEY_KP_4               Chroma::Input::Key::KP_4
#define CHROMA_KEY_KP_5               Chroma::Input::Key::KP_5
#define CHROMA_KEY_KP_6               Chroma::Input::Key::KP_6
#define CHROMA_KEY_KP_7               Chroma::Input::Key::KP_7
#define CHROMA_KEY_KP_8               Chroma::Input::Key::KP_8
#define CHROMA_KEY_KP_9               Chroma::Input::Key::KP_9
#define CHROMA_KEY_KP_DECIMAL         Chroma::Input::Key::KP_DECIMAL
#define CHROMA_KEY_KP_DIVIDE          Chroma::Input::Key::KP_DIVIDE
#define CHROMA_KEY_KP_MULTIPLY        Chroma::Input::Key::KP_MULTIPLY
#define CHROMA_KEY_KP_SUBTRACT        Chroma::Input::Key::KP_SUBTRACT
#define CHROMA_KEY_KP_ADD             Chroma::Input::Key::KP_ADD
#define CHROMA_KEY_KP_ENTER           Chroma::Input::Key::KP_ENTER
#define CHROMA_KEY_KP_EQUAL           Chroma::Input::Key::KP_EQUAL
#define CHROMA_KEY_LEFT_SHIFT         Chroma::Input::Key::LEFT_SHIFT
#define CHROMA_KEY_LEFT_CONTROL       Chroma::Input::Key::LEFT_CONTROL
#define CHROMA_KEY_LEFT_ALT           Chroma::Input::Key::LEFT_ALT
#define CHROMA_KEY_LEFT_SUPER         Chroma::Input::Key::LEFT_SUPER
#define CHROMA_KEY_RIGHT_SHIFT        Chroma::Input::Key::RIGHT_SHIFT
#define CHROMA_KEY_RIGHT_CONTROL      Chroma::Input::Key::RIGHT_CONTROL
#define CHROMA_KEY_RIGHT_ALT          Chroma::Input::Key::RIGHT_ALT
#define CHROMA_KEY_RIGHT_SUPER        Chroma::Input::Key::RIGHT_SUPER
#define CHROMA_KEY_MENU               Chroma::Input::Key::MENU

#define CHROMA_KEY_LAST               CHROMA_KEY_MENU


#define CHROMA_MOUSE_BUTTON_1         Chroma::Input::Mouse::BUTTON_1
#define CHROMA_MOUSE_BUTTON_2         Chroma::Input::Mouse::BUTTON_2
#define CHROMA_MOUSE_BUTTON_3         Chroma::Input::Mouse::BUTTON_3
#define CHROMA_MOUSE_BUTTON_4         Chroma::Input::Mouse::BUTTON_4
#define CHROMA_MOUSE_BUTTON_5         Chroma::Input::Mouse::BUTTON_5
#define CHROMA_MOUSE_BUTTON_6         Chroma::Input::Mouse::BUTTON_6
#define CHROMA_MOUSE_BUTTON_7         Chroma::Input::Mouse::BUTTON_7
#define CHROMA_MOUSE_BUTTON_8         Chroma::Input::Mouse::BUTTON_8
#define CHROMA_MOUSE_BUTTON_LAST      Chroma::Input::Mouse::LAST
#define CHROMA_MOUSE_BUTTON_LEFT      Chroma::Input::Mouse::LEFT
#define CHROMA_MOUSE_BUTTON_RIGHT     Chroma::Input::Mouse::RIGHT
#define CHROMA_MOUSE_BUTTON_MIDDLE    Chroma::Input::Mouse::MIDDLE
