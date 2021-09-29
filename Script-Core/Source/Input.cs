using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

using Joystick = Chroma.Input.Gamepad;

namespace Chroma
{
	public static class Input
	{
		public enum Mouse
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
		}

		public enum Key
		{
			/// PRINTABLE KEYS
			SPACE = 32,
			APOSTROPHE = 39, /* ' */
			COMMA = 44, /* , */
			MINUS = 45, /* - */
			PERIOD = 46, /* . */
			SLASH = 47, /* / */
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
			SEMICOLON = 59, /* ; */
			EQUAL = 61, /* = */
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
			LEFT_BRACKET = 91, /* [ */
			BACKSLASH = 92, /* \ */
			RIGHT_BRACKET = 93, /* ] */
			GRAVE_ACCENT = 96, /* ` */
			WORLD_1 = 161, /* non-US #1 */
			WORLD_2 = 162, /* non-US #2 */

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

		public enum Gamepad
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

		public enum GamepadButton
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

		public enum GamepadAxis
		{
			LEFT_X = 0,
			LEFT_Y = 1,
			RIGHT_X = 2,
			RIGHT_Y = 3,
			LEFT_TRIGGER = 4,
			RIGHT_TRIGGER = 5,
			_LAST = RIGHT_TRIGGER
		};

		public enum ConnectionState
		{
			DISCONNECTED = 0,
			CONNECTED = 1
		};

		public enum JoystickHatState
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


		private static Joystick PrimaryGamepad = Joystick._1;

		public static bool IsPressed(Key key)
		{
			return IsKeyPressed_Native(key);
		}

		public static bool IsPressed(Mouse button)
		{
			return IsMouseButtonPressed_Native(button);
		}

		public static bool IsPressed(GamepadButton button)
		{
			return IsPressed(button, PrimaryGamepad);
		}

		public static bool IsPressed(GamepadButton button, Gamepad gamepad)
		{
			return GetGamepadButtonPressed_Native(gamepad, button);
		}




		public static Vector2 GetMousePos()
		{
			Vector2 res;
			GetMousePosition_Native(out res);
			return res;
		}

		public static float GetMouseX()
		{
			return GetMousePos().X;
		}

		public static float GetMouseY()
		{
			return GetMousePos().Y;
		}

		public static bool IsJoystickConnected()
		{
			return IsJoystickConnected(PrimaryGamepad);
		}

		public static bool IsJoystickConnected(Gamepad gamepad)
		{
			return IsJoystickConnected_Native(gamepad);
		}

		public static Gamepad GetFirstConnectedGamepad()
		{
			return GetFirstConnectedGamepad_Native();
		}

		public static List<Gamepad> GetAllConnectedControllers()
		{
			List<Gamepad> res;
			GetAllConnectedControllers(out res);
			return res;
		}

		public static Gamepad SetPrimaryGamepad()
		{
			return SetPrimaryGamepad_Native();
		}

		public static List<float> GetGamepadAxis()
		{
			return GetGamepadAxis(PrimaryGamepad);
		}

		public static List<float> GetGamepadAxis(Gamepad gamepad)
		{
			List<float> res;
			GetGamepadAxis_Native(gamepad, out res);
			return res;
		}

		public static ConnectionState GetGamepadConnectionState()
		{
			return GetGamepadConnectionState(PrimaryGamepad);
		}

		public static ConnectionState GetGamepadConnectionState(Gamepad gamepad)
		{
			return GetGamepadConnectionState_Native(gamepad);
		}

		public static string GetGamepadName()
		{
			return GetGamepadName(PrimaryGamepad);
		}

		public static string GetGamepadName(Gamepad gamepad)
		{
			return GetGamepadName(gamepad);
		}

		public static void SetGamepadConnectionCallback(Action<Gamepad> action)
		{

		}

		public static void SetGamepadDisconnectionCallback(Action<Gamepad> action)
		{

		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool IsKeyPressed_Native(Key key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool IsMouseButtonPressed_Native(Mouse button);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool IsJoystickConnected_Native(Joystick stick);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void GetMousePosition_Native(out Vector2 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern Gamepad GetFirstConnectedGamepad_Native();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void GetAllConnectedControllers(out List<Gamepad> gamepads);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern Gamepad SetPrimaryGamepad_Native();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void GetGamepadAxis_Native(Gamepad gamepad, out List<float> result);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool GetGamepadButtonPressed_Native(Gamepad gamepad, GamepadButton button);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern ConnectionState GetGamepadConnectionState_Native(Gamepad gamepad);


	}

}
