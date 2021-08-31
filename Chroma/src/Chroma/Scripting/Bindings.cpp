#include "chromapch.h"
#include "Bindings.h"

#include "Chroma/Math/Math.h"
#include "glm/gtx/string_cast.hpp"
#include <sol/sol.hpp>
#include <fmt/format.h>

#include "Chroma/Scene/Entity.h"
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/BoxCollider2D.h>
#include <Chroma/Components/CircleCollider2D.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Core/Input.h>
#include <Chroma/Core/Log.h>
#include <Chroma/Scripting/LuaScripting.h>


namespace Chroma
{

	void Bindings::BindAllTheThings(sol::state_view& lua)
	{
		BindMath(lua);
		BindEntity(lua);
		BindComponents(lua);
		BindTime(lua);
		BindInput(lua);
		lua.set_function("log", [](const std::string& fmt) { CHROMA_TRACE(fmt); });
		lua.set_function("log_info", [](const std::string& fmt) { CHROMA_INFO(fmt); });
		lua.set_function("log_warn", [](const std::string& fmt) { CHROMA_WARN(fmt); });
		lua.set_function("log_error", [](const std::string& fmt) { CHROMA_ERROR(fmt); });
		lua.set_function("log_critical", [](const std::string& fmt) { CHROMA_CRITICAL(fmt); });
		lua.set_function("include", [&](const std::string& file) 
		{ 
			//TODO - Needs to load the module into the correct state, which means it needs
		});

	}

	void Bindings::BindEntity(sol::state_view& lua)
	{
		lua.new_usertype<Entity>("Entity",

			"Entity", sol::no_constructor,

			//Properties
			"ID", sol::readonly_property(&Entity::GetID),
			"Scene", sol::readonly_property(&Entity::GetScene),
			"Name", sol::property(&Entity::GetName, &Entity::SetName),

			"PrintScenePointer", [](Entity& e) { CHROMA_CORE_INFO("Scene Pointer: {:p}", (void*)e.m_Scene); },

			//Methods
			"Valid", [](Entity& e) {return &e.GetScene() != nullptr && e.GetID() != ENTITY_NULL; },

			"HasComponent", [](Entity& e, const std::string& component) {return e.HasComponent(component); },
			"HasAudioSource", & Entity::HasComponent<AudioSource>,
			"HasBoxCollider2D", &Entity::HasComponent<BoxCollider2D>,
			"HasCircleCollider2D", &Entity::HasComponent<CircleCollider2D>,
			"HasSpriteRenderer", &Entity::HasComponent<SpriteRenderer>,
			
			"AddComponent", [](Entity& e, const std::string& component) {return e.AddComponent(component); },
			"AddAudioSource", [](Entity& e) { return std::ref(e.AddComponent<AudioSource>()); },
			"AddBoxCollider2D", [](Entity& e) { return std::ref(e.AddComponent<BoxCollider2D>()); },
			"AddCircleCollider2D", [](Entity& e) { return std::ref(e.AddComponent<CircleCollider2D>()); },
			"AddSpriteRenderer", [](Entity& e) { return std::ref(e.AddComponent<SpriteRenderer>()); },

			"GetComponent", [](Entity& e, const std::string& component) { return e.GetComponent(component); },
			"GetTag", [](Entity& e) { return std::ref(e.GetComponent<Tag>()); },
			"GetTransform", [](Entity& e) { return std::ref(e.GetComponent<Transform>()); },
			"GetAudioSource", [](Entity& e) { return std::ref(e.GetComponent<AudioSource>()); },
			"GetBoxCollider2D", [](Entity& e) { return std::ref(e.GetComponent<BoxCollider2D>()); },
			"GetCircleCollider2D", [](Entity& e) { return std::ref(e.GetComponent<CircleCollider2D>()); },
			"GetSpriteRenderer", [](Entity& e) { return std::ref(e.GetComponent<SpriteRenderer>()); },


			//Metamethods
			sol::meta_function::to_string, [](Entity& e) { return fmt::format("{} (Entity {})", e.GetName(), e.GetID()); },
			sol::meta_function::equal_to, &Entity::operator==,
			sol::meta_function::less_than, &Entity::operator<

		);
	}

	void Bindings::BindComponents(sol::state_view& lua)
	{
		lua.new_usertype<Component>("Component",
			"Component", sol::no_constructor,

			//Properties
			"ID", sol::readonly_property(&Component::GetUniqueID),
			"AllowsMultiple", sol::readonly_property(&Component::AllowMultiple),
			"Name", sol::readonly_property(&Component::Name)
		);

		lua.new_usertype<AudioSource>("AudioSource",
			"AudioSource", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			//Members
			"Event", &AudioSource::Event,
			"Mute", &AudioSource::Mute,
			"PlayOnInit", &AudioSource::PlayOnInit

		);

		lua.new_usertype<BoxCollider2D>("BoxCollider2D",
			"BoxCollider2D", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			"Bounds", &BoxCollider2D::Bounds,
			"Offset", &BoxCollider2D::Offset

		);

	
		lua.new_usertype<CircleCollider2D>("CircleCollider2D",
			"CircleCollider2D", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			"Radius", &CircleCollider2D::Radius,
			"Offset", &CircleCollider2D::Offset
		);
	

		lua.new_usertype<SpriteRenderer>("SpriteRenderer",
			"SpriteRenderer", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),

			//Properties
			"RenderColor", &SpriteRenderer::Color,
			"Offset", &SpriteRenderer::Offset,
			"Layer", &SpriteRenderer::Layer,
			"PlayOnStart", &SpriteRenderer::PlayOnStart,
			"Playing", &SpriteRenderer::Playing,
			"Loop", &SpriteRenderer::Loop,
			"SpeedMultipllier", &SpriteRenderer::SpeedMultiplier,

			//Methods
			"GetSpriteID", &SpriteRenderer::GetSpriteID,
			"SetSprite", &SpriteRenderer::SetSprite,
			"SetAnimation", [](SpriteRenderer& renderer, const std::string& animation) {renderer.SetAnimation(animation); },
			"GetAnimation", &SpriteRenderer::GetAnimation,
			"GetAnimationName", &SpriteRenderer::GetAnimationName,
			"GetCurrentFrame", &SpriteRenderer::GetCurrentFrame,
			"SetCurrentFrame", &SpriteRenderer::SetCurrentFrame,
			"RestartAnimation", &SpriteRenderer::RestartAnimation
		);


		lua.new_usertype<Transform>("Transform",
			"Transform", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),

			"Position", &Transform::Position,
			"Rotation", &Transform::Rotation,
			"Scale", &Transform::Scale

		);
	}

	void Bindings::BindScene(sol::state_view& lua)
	{
		
	}

	void Bindings::BindTime(sol::state_view& lua)
	{
		lua.new_usertype<Chroma::Time>("Time",
			"Time", sol::no_constructor,

			"Delta", sol::readonly_property(&Time::GetSeconds)
		);
	}

	void Bindings::BindInput(sol::state_view& lua)
	{
		lua.new_enum<Chroma::Input::Mouse>("Mouse", {
			{"BUTTON_1",	Input::Mouse::BUTTON_1		},
			{"BUTTON_2",	Input::Mouse::BUTTON_2		},
			{"BUTTON_3",	Input::Mouse::BUTTON_3		},
			{"BUTTON_4",	Input::Mouse::BUTTON_4		},
			{"BUTTON_5",	Input::Mouse::BUTTON_5		},
			{"BUTTON_6",	Input::Mouse::BUTTON_6		},
			{"BUTTON_7",	Input::Mouse::BUTTON_7		},
			{"BUTTON_8",	Input::Mouse::BUTTON_8		},
			{"LAST"	   ,	Input::Mouse::_LAST			},
			{"LEFT"	   ,	Input::Mouse::LEFT,			},
			{"RIGHT"   ,	Input::Mouse::RIGHT,		},
			{"MIDDLE"  ,	Input::Mouse::MIDDLE		}}
		);

		lua.new_enum<Chroma::Input::Key>("Key", {
			{"SPACE", Input::Key::SPACE},
			{"APOSTROPHE", Input::Key::APOSTROPHE},
			{"COMMA", Input::Key::COMMA},
			{"MINUS", Input::Key::MINUS},
			{"PERIOD", Input::Key::PERIOD},
			{"SLASH", Input::Key::SLASH},
			{"K_0", Input::Key::K_0},
			{"K_1", Input::Key::K_1},
			{"K_2", Input::Key::K_2},
			{"K_3", Input::Key::K_3},
			{"K_4", Input::Key::K_4},
			{"K_5", Input::Key::K_5},
			{"K_6", Input::Key::K_6},
			{"K_7", Input::Key::K_7},
			{"K_8", Input::Key::K_8},
			{"K_9", Input::Key::K_9},
			{"SEMICOLON", Input::Key::SEMICOLON},
			{"EQUAL", Input::Key::EQUAL},
			{"A", Input::Key::A},
			{"B", Input::Key::B},
			{"C", Input::Key::C},
			{"D", Input::Key::D},
			{"E", Input::Key::E},
			{"F", Input::Key::F},
			{"G", Input::Key::G},
			{"H", Input::Key::H},
			{"I", Input::Key::I},
			{"J", Input::Key::J},
			{"K", Input::Key::K},
			{"L", Input::Key::L},
			{"M", Input::Key::M},
			{"N", Input::Key::N},
			{"O", Input::Key::O},
			{"P", Input::Key::P},
			{"Q", Input::Key::Q},
			{"R", Input::Key::R},
			{"S", Input::Key::S},
			{"T", Input::Key::T},
			{"U", Input::Key::U},
			{"V", Input::Key::V},
			{"W", Input::Key::W},
			{"X", Input::Key::X},
			{"Y", Input::Key::Y},
			{"Z", Input::Key::Z},
			{"LEFT_BRACKET", Input::Key::LEFT_BRACKET},
			{"BACKSLASH", Input::Key::BACKSLASH},
			{"RIGHT_BRACKET", Input::Key::RIGHT_BRACKET},
			{"GRAVE_ACCENT", Input::Key::GRAVE_ACCENT},
			{"WORLD_1", Input::Key::WORLD_1},
			{"WORLD_2", Input::Key::WORLD_2},

			{"ESCAPE", Input::Key::ESCAPE},
			{"ENTER", Input::Key::ENTER},
			{"TAB", Input::Key::TAB},
			{"BACKSPACE", Input::Key::BACKSPACE},
			{"INSERT", Input::Key::INSERT},
			{"DEL", Input::Key::DEL},
			{"RIGHT", Input::Key::RIGHT},
			{"LEFT", Input::Key::LEFT},
			{"DOWN", Input::Key::DOWN},
			{"UP", Input::Key::UP},
			{"PAGE_UP", Input::Key::PAGE_UP},
			{"PAGE_DOWN", Input::Key::PAGE_DOWN},
			{"HOME", Input::Key::HOME},
			{"END", Input::Key::END},
			{"CAPS_LOCK", Input::Key::CAPS_LOCK},
			{"SCROLL_LOCK", Input::Key::SCROLL_LOCK},
			{"NUM_LOCK", Input::Key::NUM_LOCK},
			{"PRINT_SCREEN", Input::Key::PRINT_SCREEN},
			{"PAUSE", Input::Key::PAUSE},
			{"F1", Input::Key::F1},
			{"F2", Input::Key::F2},
			{"F3", Input::Key::F3},
			{"F4", Input::Key::F4},
			{"F5", Input::Key::F5},
			{"F6", Input::Key::F6},
			{"F7", Input::Key::F7},
			{"F8", Input::Key::F8},
			{"F9", Input::Key::F9},
			{"F10", Input::Key::F10},
			{"F11", Input::Key::F11},
			{"F12", Input::Key::F12},
			{"F13", Input::Key::F13},
			{"F14", Input::Key::F14},
			{"F15", Input::Key::F15},
			{"F16", Input::Key::F16},
			{"F17", Input::Key::F17},
			{"F18", Input::Key::F18},
			{"F19", Input::Key::F19},
			{"F20", Input::Key::F20},
			{"F21", Input::Key::F21},
			{"F22", Input::Key::F22},
			{"F23", Input::Key::F23},
			{"F24", Input::Key::F24},
			{"F25", Input::Key::F25},
			{"KP_0", Input::Key::KP_0},
			{"KP_1", Input::Key::KP_1},
			{"KP_2", Input::Key::KP_2},
			{"KP_3", Input::Key::KP_3},
			{"KP_4", Input::Key::KP_4},
			{"KP_5", Input::Key::KP_5},
			{"KP_6", Input::Key::KP_6},
			{"KP_7", Input::Key::KP_7},
			{"KP_8", Input::Key::KP_8},
			{"KP_9", Input::Key::KP_9},
			{"KP_DECIMAL", Input::Key::KP_DECIMAL},
			{"KP_DIVIDE", Input::Key::KP_DIVIDE},
			{"KP_MULTIPLY", Input::Key::KP_MULTIPLY},
			{"KP_SUBTRACT", Input::Key::KP_SUBTRACT},
			{"KP_ADD", Input::Key::KP_ADD},
			{"KP_ENTER", Input::Key::KP_ENTER},
			{"KP_EQUAL", Input::Key::KP_EQUAL},
			{"LEFT_SHIFT", Input::Key::LEFT_SHIFT},
			{"LEFT_CONTROL", Input::Key::LEFT_CONTROL},
			{"LEFT_ALT", Input::Key::LEFT_ALT},
			{"LEFT_SUPER", Input::Key::LEFT_SUPER},
			{"RIGHT_SHIFT", Input::Key::RIGHT_SHIFT},
			{"RIGHT_CONTROL", Input::Key::RIGHT_CONTROL},
			{"RIGHT_ALT", Input::Key::RIGHT_ALT},
			{"RIGHT_SUPER", Input::Key::RIGHT_SUPER},
			{"MENU", Input::Key::MENU},

			{"_LAST", Input::Key::_LAST}
		});

		lua.new_enum<Chroma::Input::Joystick>("Joystick", {
				{"_1",Input::Joystick::_1},
				{"_2",Input::Joystick::_2},
				{"_3",Input::Joystick::_3},
				{"_4",Input::Joystick::_4},
				{"_5",Input::Joystick::_5},
				{"_6",Input::Joystick::_6},
				{"_7",Input::Joystick::_7},
				{"_8",Input::Joystick::_8},
				{"_9",Input::Joystick::_9},
				{"_10",Input::Joystick::_10},
				{"_11",Input::Joystick::_11},
				{"_12",Input::Joystick::_12},
				{"_13",Input::Joystick::_13},
				{"_14",Input::Joystick::_14},
				{"_15",Input::Joystick::_15},
				{"_16",Input::Joystick::_16},
				{"_LAST",Input::Joystick::_LAST},
				{"_NULL",Input::Joystick::_NULL}
		});

		lua.new_enum<Chroma::Input::Gamepad>("Gamepad", {
				{"_1",Input::Gamepad::_1},
				{"_2",Input::Gamepad::_2},
				{"_3",Input::Gamepad::_3},
				{"_4",Input::Gamepad::_4},
				{"_5",Input::Gamepad::_5},
				{"_6",Input::Gamepad::_6},
				{"_7",Input::Gamepad::_7},
				{"_8",Input::Gamepad::_8},
				{"_9",Input::Gamepad::_9},
				{"_10",Input::Gamepad::_10},
				{"_11",Input::Gamepad::_11},
				{"_12",Input::Gamepad::_12},
				{"_13",Input::Gamepad::_13},
				{"_14",Input::Gamepad::_14},
				{"_15",Input::Gamepad::_15},
				{"_16",Input::Gamepad::_16},
				{"_LAST",Input::Gamepad::_LAST},
				{"_NULL",Input::Gamepad::_NULL}
			});

		lua.new_enum<Chroma::Input::ButtonState>("ButtonState", {
			{"PRESSED", Chroma::Input::ButtonState::PRESSED},
			{"RELEASED", Chroma::Input::ButtonState::RELEASED}
		});

		lua.new_enum<Chroma::Input::GamepadButton>("GamepadButton", {
			{"A",Input::GamepadButton::A},
			{"B",Input::GamepadButton::B},
			{"X",Input::GamepadButton::X},
			{"Y",Input::GamepadButton::Y},
			{"LEFT_BUMPER",Input::GamepadButton::LEFT_BUMPER},
			{"RIGHT_BUMPER",Input::GamepadButton::RIGHT_BUMPER},
			{"BACK",Input::GamepadButton::BACK},
			{"START",Input::GamepadButton::START},
			{"GUIDE",Input::GamepadButton::GUIDE},
			{"LEFT_THUMB",Input::GamepadButton::LEFT_THUMB},
			{"RIGHT_THUMB",Input::GamepadButton::RIGHT_THUMB},
			{"DPAD_UP",Input::GamepadButton::DPAD_UP},
			{"DPAD_RIGHT",Input::GamepadButton::DPAD_RIGHT},
			{"DPAD_DOWN",Input::GamepadButton::DPAD_DOWN},
			{"DPAD_LEFT",Input::GamepadButton::DPAD_LEFT},
			{"_LAST",Input::GamepadButton::_LAST},
			{"CROSS",Input::GamepadButton::CROSS},
			{"CIRCLE",Input::GamepadButton::CIRCLE},
			{"SQUARE",Input::GamepadButton::SQUARE},
			{"TRIANGLE",Input::GamepadButton::TRIANGLE}
		});

		lua.new_enum<Chroma::Input::GamepadButton>("JoystickButton", {
			{"A",Input::JoystickButton::A},
			{"B",Input::JoystickButton::B},
			{"X",Input::JoystickButton::X},
			{"Y",Input::JoystickButton::Y},
			{"LEFT_BUMPER",Input::JoystickButton::LEFT_BUMPER},
			{"RIGHT_BUMPER",Input::JoystickButton::RIGHT_BUMPER},
			{"BACK",Input::JoystickButton::BACK},
			{"START",Input::JoystickButton::START},
			{"GUIDE",Input::JoystickButton::GUIDE},
			{"LEFT_THUMB",Input::JoystickButton::LEFT_THUMB},
			{"RIGHT_THUMB",Input::JoystickButton::RIGHT_THUMB},
			{"DPAD_UP",Input::JoystickButton::DPAD_UP},
			{"DPAD_RIGHT",Input::JoystickButton::DPAD_RIGHT},
			{"DPAD_DOWN",Input::JoystickButton::DPAD_DOWN},
			{"DPAD_LEFT",Input::JoystickButton::DPAD_LEFT},
			{"_LAST",Input::JoystickButton::_LAST},
			{"CROSS",Input::JoystickButton::CROSS},
			{"CIRCLE",Input::JoystickButton::CIRCLE},
			{"SQUARE",Input::JoystickButton::SQUARE},
			{"TRIANGLE",Input::JoystickButton::TRIANGLE}
			});

		lua.new_enum<Chroma::Input::GamepadAxis>("GamepadAxis", {
				{"LEFT_X",Input::GamepadAxis::LEFT_X},
				{"LEFT_Y",Input::GamepadAxis::LEFT_Y},
				{"RIGHT_X",Input::GamepadAxis::RIGHT_X},
				{"RIGHT_Y",Input::GamepadAxis::RIGHT_Y},
				{"LEFT_TRIGGER",Input::GamepadAxis::LEFT_TRIGGER},
				{"RIGHT_TRIGGER",Input::GamepadAxis::RIGHT_TRIGGER},
				{"_LAST",Input::GamepadAxis::_LAST},
			});

		lua.new_enum<Chroma::Input::ConnectionState>("ConnectionState", {
			{"DISCONNECTED",Input::ConnectionState::DISCONNECTED},
			{"CONNECTED",Input::ConnectionState::CONNECTED}
		});

		lua.new_enum<Chroma::Input::JoystickHatState>("JoystickHatState", {
			{"CENTERED",Input::JoystickHatState::CENTERED},
			{"UP",Input::JoystickHatState::UP},
			{"RIGHT",Input::JoystickHatState::RIGHT},
			{"DOWN",Input::JoystickHatState::DOWN},
			{"LEFT",Input::JoystickHatState::LEFT},
			{"RIGHT_UP",Input::JoystickHatState::RIGHT_UP},
			{"RIGHT_DOWN",Input::JoystickHatState::RIGHT_DOWN},
			{"LEFT_UP",Input::JoystickHatState::LEFT_UP},
			{"LEFT_DOWN",Input::JoystickHatState::LEFT_DOWN}
		});


		lua.new_usertype<Chroma::Input::GamepadState>("GamepadState",
			"GamepadState", sol::no_constructor,
			"ButtonCount", sol::readonly_property([]() { return Chroma::Input::GamepadState::ButtonCount; }),
			"AxisCount", sol::readonly_property([]() { return Chroma::Input::GamepadState::AxisCount; }),
			"GetButtonState", &Chroma::Input::GamepadState::GetButtonState,
			"GetAxisState", &Chroma::Input::GamepadState::GetAxisState,


			sol::meta_function::index, [](Input::GamepadState state, Input::GamepadAxis axis) { return state[axis]; },
			sol::meta_function::index, sol::overload([](Input::GamepadState state, Input::GamepadButton button) { return state[button]; })
		);

		

		lua.new_usertype<Chroma::Input>("InputType",
			"InputType", sol::no_constructor,
			"IsKeyPressed", [](Chroma::Input input, Chroma::Input::Key key) { return Chroma::Input::IsKeyPressed(key); },
			"IsMouseButtonPressed", [](Chroma::Input input, Chroma::Input::Mouse mouse) { return Chroma::Input::IsMouseButtonPressed(mouse); },
			"GetMousePos", [](Chroma::Input input) { return Chroma::Input::GetMousePos(); },
			"GetMouseX", [](Chroma::Input input) { return Chroma::Input::GetMouseX(); },
			"GetMouseY", [](Chroma::Input input) { return Chroma::Input::GetMouseY(); },


			"IsJoystickConnected", [](Chroma::Input input, Chroma::Input::Joystick joystick) { return Chroma::Input::IsJoystickConnected(joystick); },
			"IsJoystickConnected", sol::overload([](Chroma::Input input) { return Chroma::Input::IsJoystickConnected(); }),
			"IsGamepad", [](Chroma::Input input, Chroma::Input::Gamepad gamepad) { return Chroma::Input::IsGamepad(gamepad); },
			"IsGamepad", sol::overload([](Chroma::Input input) { return Chroma::Input::IsGamepad(); }),
			"GetFirstConnectedGamepad", [](Chroma::Input input) { return Chroma::Input::GetFirstConnectedGamepad(); },
			"GetFirstConnectedJoystick", [](Chroma::Input input) { return Chroma::Input::GetFirstConnectedJoystick(); },
			"GetAllConnectedControllers", [](Chroma::Input input) { return Chroma::Input::GetAllConnectedControllers(); },
			"GetJoystickHats", [](Chroma::Input input, Chroma::Input::Joystick joystick, int* count) { return Chroma::Input::GetJoystickHats(joystick, count); },

			"SetPrimaryGamepad", [](Chroma::Input input) { return Chroma::Input::SetPrimaryGamepad(); },

			"GetGamepadAxis", [](Chroma::Input input) { return Chroma::Input::GetGamepadAxis(); },
			"GetGamepadAxis", sol::overload([](Chroma::Input input, Chroma::Input::Gamepad gamepad) { return Chroma::Input::GetGamepadAxis(gamepad); }),
			"GetGamepadButtonState", [](Chroma::Input input, Chroma::Input::GamepadButton button) { return Chroma::Input::GetGamepadButtonState(button); },
			"GetGamepadButtonState", sol::overload([](Chroma::Input input, Chroma::Input::GamepadButton button, Chroma::Input::Gamepad gamepad) { return Chroma::Input::GetGamepadButtonState(button, gamepad); }),
			"GetJoystickButtonState", [](Chroma::Input input, Chroma::Input::JoystickButton button) { return Chroma::Input::GetJoystickButtonState(button); },
			"GetJoystickButtonState", sol::overload([](Chroma::Input input, Chroma::Input::JoystickButton button, Chroma::Input::Joystick joystick) { return Chroma::Input::GetJoystickButtonState(button, joystick); }),

			"GetGamepadState", [](Chroma::Input input) { return Chroma::Input::GetGamepadState(); },
			"GetGamepadState", sol::overload([](Chroma::Input input, Chroma::Input::Gamepad gamepad) { return Chroma::Input::GetGamepadState(gamepad); }),
			"GetGamepadConnectionState", [](Chroma::Input input) { return  Chroma::Input::GetGamepadConnectionState(); },
			"GetGamepadConnectionState", sol::overload([](Chroma::Input input, Chroma::Input::Gamepad gamepad) { return  Chroma::Input::GetGamepadConnectionState(gamepad); }),

			"GetGamepadName", [](Chroma::Input input) { return Chroma::Input::GetGamepadName(); },
			"GetGamepadName", sol::overload([](Chroma::Input input, Chroma::Input::Gamepad gamepad) { return Chroma::Input::GetGamepadName(gamepad); }),

			"SetGamepadConnectionCallback", [](Chroma::Input input, std::function<void(Chroma::Input::Gamepad)> func) { return Chroma::Input::SetGamepadConnectionCallback(func); },
			"SetGamepadDisconnectionCallback", [](Chroma::Input input, std::function<void(Chroma::Input::Gamepad)> func) { return Chroma::Input::SetGamepadDisconnectionCallback(func); },

			"PrimaryGamepad", sol::property([](Chroma::Input input) {return Chroma::Input::PrimaryGamepad; }, [](Chroma::Input input, Chroma::Input::Gamepad gamepad) { Chroma::Input::PrimaryGamepad = gamepad; })
		);
	}

	void Bindings::BindMath(sol::state_view& lua)
	{
		BindVec2(lua);
		BindIVec2(lua);
		BindVec3(lua);
		BindIVec3(lua);
		BindVec4(lua);
		BindIVec4(lua);
	}

	void Bindings::BindVec2(sol::state_view& lua)
	{
		lua.new_usertype<Math::vec2>("Vec2",

			//Members
			"X", &Math::vec2::x,
			"Y", &Math::vec2::y,

			//Methods
			"Length", &Math::vec2::length,
			"Normalize", [](Math::vec2& v) { v = glm::normalize(v); return v; },
			"Distance", [](Math::vec2& v1, Math::vec2& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::vec2& v) {return fmt::format("Vec2({}, {})", v.x, v.y); },
			sol::meta_function::index, [](Math::vec2& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
			sol::meta_function::new_index, [](Math::vec2& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator==<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator-<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator+<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator/<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator*<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::vec2& v1) { return glm::operator-<float, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua.set_function("Vec2", []() {return Math::ivec2(); });
		lua.set_function("Vec2", sol::overload([](float x, float y) {return Math::vec2(x, y); }));

		//Static functions
		lua.set_function("Vec2_Distance", [](Math::vec2& v1, Math::vec2& v2) { return glm::distance(v1, v2); });

	}

	void Bindings::BindIVec2(sol::state_view& lua)
	{
		lua.new_usertype<Math::ivec2>("IVec2",

			//Members
			"X", &Math::ivec2::x,
			"Y", &Math::ivec2::y,

			//Methods
			//"Length", &Math::ivec2::length,
			//"Distance", [](Math::ivec2& v1, Math::ivec2& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::ivec2& v) {return fmt::format("IVec2({}, {})", v.x, v.y); },
			sol::meta_function::index, [](Math::ivec2& v, const int index) { if (index < 0 or index > 2) return 0; return v[index]; },
			sol::meta_function::new_index, [](Math::ivec2& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator==<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator-<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator+<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator/<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator*<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::ivec2& v1) { return glm::operator-<int, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua.set_function("IVec2", []() {return Math::ivec2(); });
		lua.set_function("IVec2", sol::overload([](int x, int y, int z) {return Math::ivec2(x, y); }));

		//Static functions
		//lua.set_function("IVec2_Distance", [](Math::ivec2& v1, Math::ivec2& v2) { return glm::distance(v1, v2); });
	}

	void Bindings::BindVec3(sol::state_view& lua)
	{
		lua.new_usertype<Math::vec3>("Vec3",

			//Members
			"X", &Math::vec3::x,
			"Y", &Math::vec3::y,
			"Z", &Math::vec3::z,

			//Methods
			"Length", &Math::vec3::length,
			"Cross", [](Math::vec3& v1, Math::vec3& v2) { return glm::cross(v1, v2); },
			"Normalize", [](Math::vec3& v) { v = glm::normalize(v); return v; },
			"Distance", [](Math::vec3& v1, Math::vec3& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::vec3& v) {return fmt::format("Vec3({}, {}, {})", v.x, v.y, v.z); },
			sol::meta_function::index, [](Math::vec3& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
			sol::meta_function::new_index, [](Math::vec3& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator==<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator-<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator+<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator/<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator*<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::vec3& v1) { return glm::operator-<float, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua.set_function("Vec3", []() {return Math::ivec3(); });
		lua.set_function("Vec3", sol::overload([](float x, float y, float z) {return Math::vec3(x, y, z); }));

		//Static functions
		lua.set_function("Vec3_Distance", [](Math::vec3& v1, Math::vec3& v2) { return glm::distance(v1, v2); });
		lua.set_function("Vec3_Cross", [](Math::vec3& v1, Math::vec3& v2) { return glm::cross(v1, v2); });
	}

	void Bindings::BindIVec3(sol::state_view& lua)
	{

		lua.new_usertype<Math::ivec3>("IVec3",

			//Members
			"X", &Math::ivec3::x,
			"Y", &Math::ivec3::y,
			"Z", &Math::ivec3::z,

			//Methods
			//"Length", &Math::ivec3::length,
			//"Distance", [](Math::ivec3& v1, Math::ivec3& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::ivec3& v) {return fmt::format("IVec3({}, {}, {})", v.x, v.y, v.z); },
			sol::meta_function::index, [](Math::ivec3& v, const int index) { if (index < 0 or index > 2) return 0; return v[index]; },
			sol::meta_function::new_index, [](Math::ivec3& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator==<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator-<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator+<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator/<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator*<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::ivec3& v1) { return glm::operator-<int, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua.set_function("IVec3", []() {return Math::ivec3(); });
		lua.set_function("IVec3", sol::overload([](int x, int y, int z) {return Math::ivec3(x, y, z); }));

		//Static functions
		//lua.set_function("IVec3_Distance", [](Math::ivec3& v1, Math::ivec3& v2) { return glm::distance(v1, v2); });
	}

	void Bindings::BindVec4(sol::state_view& lua)
	{
		lua.new_usertype<Math::vec4>("Vec4",

			//Members
			"X", &Math::vec4::x,
			"Y", &Math::vec4::y,
			"Z", &Math::vec4::z,
			"W", &Math::vec4::w,


			//Methods
			"Length", &Math::vec4::length,
			"Normalize", [](Math::vec4& v) { v = glm::normalize(v); return v; },
			"Distance", [](Math::vec4& v1, Math::vec4& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::vec4& v) {return fmt::format("Vec4({}, {}, {}, {})", v.x, v.y, v.z, v.w); },
			sol::meta_function::index, [](Math::vec4& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
			sol::meta_function::new_index, [](Math::vec4& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator==<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator-<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator+<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator/<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator*<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::vec4& v1) { return glm::operator-<float, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua.set_function("Vec4", []() {return Math::ivec4(); });
		lua.set_function("Vec4", sol::overload([](float x, float y, float z, float w) {return Math::vec4(x, y, z, w); }));

		//Static functions
		lua.set_function("Vec4_Distance", [](Math::vec4& v1, Math::vec4& v2) { return glm::distance(v1, v2); });
	}

	void Bindings::BindIVec4(sol::state_view& lua)
	{

		lua.new_usertype<Math::ivec4>("IVec4",

			//Members
			"X", &Math::ivec4::x,
			"Y", &Math::ivec4::y,
			"Z", &Math::ivec4::z,
			"W", &Math::ivec4::w,

			//Methods
			//"Length", &Math::ivec4::length,
			//"Distance", [](Math::ivec4& v1, Math::ivec4& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::ivec4& v) {return fmt::format("IVec4({}, {}, {}, {})", v.x, v.y, v.z, v.w); },
			sol::meta_function::index, [](Math::ivec4& v, const int index) { if (index < 0 or index > 2) return 0; return v[index]; },
			sol::meta_function::new_index, [](Math::ivec4& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator==<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator-<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator+<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator/<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator*<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::ivec4& v1) { return glm::operator-<int, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua.set_function("IVec4", []() {return Math::ivec4(); });
		lua.set_function("IVec4", sol::overload([](int x, int y, int z, int w) {return Math::ivec4(x, y, z, w); }));

		//Static functions
		//lua.set_function("IVec4_Distance", [](Math::ivec4& v1, Math::ivec4& v2) { return glm::distance(v1, v2); });
	}


}