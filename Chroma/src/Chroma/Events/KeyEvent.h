#pragma once

#include "chromapch.h"
#include "Event.h"

namespace Chroma
{
	/// @brief Event for when a key is pressed down.
	///
	/// This event only occurs once per key press.
	/// @see KeyCodes.h for list of Key Codes.
	class CHROMA_API KeyEvent : public Event
	{
	public:
		/// @brief Gets the keycode of the key pressed.
		/// @return Keycode of the key pressed.
		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		/// @brief Constructs a KeyEvent.
		KeyEvent(int keycode)
			: m_KeyCode(keycode)
		{
		}

		/// @brief Keycode of the key pressed.
		int m_KeyCode;
	};

	/// @brief Event for when a key is pressed.
	///
	/// This event repeates while being held down.
	/// This is often used for handling typing.
	/// @see KeyCodes.h for list of Key Codes.
	class CHROMA_API KeyPressedEvent : public KeyEvent
	{
	public:
		/// @brief Constructs a KeyPressedEvent
		/// @param keycode Keycode of the key pressed.
		/// @param repeatCount Number of times the key has been repeated.
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) { }

		/// @brief Gets number of repeats.
		/// @return Number of repeats.
		inline int GetRepeatCount() const { return m_RepeatCount;  }

		/// @brief Converts the KeyPressedEvent to a string.
		/// @return String containing keycode and repeat count.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		/// @brief Number of repeats.
		int m_RepeatCount;
	};

	/// @brief Event for when a key is released.
	///
	/// This event only occurs once per key press, on released.
	/// @see KeyCodes.h for list of Key Codes.
	class CHROMA_API KeyReleasedEvent : public KeyEvent
	{
	public:
		/// @brief Constructs a KeyReleasedEvent
		/// @param keycode Key code of the key pressed.
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode)
		{
		}

		/// @brief Converts the KeyReleasedEvent to a string.
		/// @return String containing keycode.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	/// @brief Event for when a key is pressed and released (typed).
	///
	/// This event only occurs once per key press.
	/// @see KeyCodes.h for list of Key Codes.
	class CHROMA_API KeyTypedEvent : public KeyEvent
	{
	public:
		/// @brief Constructs a KeyTypedEvent.
		/// @param keycode 
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode)
		{
		}

		/// @brief Converts the KeyTypedEvent to a string.
		/// @return String containing keycode.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)

	private:

	};
}
