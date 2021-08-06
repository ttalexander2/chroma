#pragma once

#include "chromapch.h"
#include "Event.h"

namespace Chroma
{

	/// @brief Mouse moved event, created when the mouse is moved.
	///
	/// Holds an x,y coordinate in pixels representing the mouse's position, relative to the window.
	class CHROMA_API MouseMovedEvent : public Event
	{
	public:
		/// @brief Constructs a MouseMovedEvent.
		/// @param x X position of the mouse.
		/// @param y Y position of the mouse.
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y)
		{
		}

		/// @brief Get the X position of the mouse.
		/// @return X position of the mouse in pixels.
		inline float GetX() const { return m_MouseX; }

		/// @brief Get the Y position of the mouse.
		/// @return Y position of the mouse in pixels.
		inline float GetY() const { return m_MouseY; }

		/// @brief Converts the MouseMovedEvent to a string.
		/// @return String containing the mouse position.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: (" << m_MouseX << ", " << m_MouseY << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		/// @brief Position of the mouse.
		float m_MouseX, m_MouseY;
	};





	/// @brief Mouse scrolled event, created when the mouse wheel is scrolled.
	///
	/// Holds an x,y coordinate in pixels representing the mouse's position, relative to the window.
	class CHROMA_API MouseScrolledEvent : public Event
	{
	public:
		/// @brief Constructs a MouseScrolledEvent
		/// @param xOffset x offset of the mouse wheel
		/// @param yOffset y offset of the mouse wheel?
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset)
		{
		}
		/// @brief Gets the X offset
		/// @return X offset in pixels.
		inline float GetXOffset() const { return m_XOffset; }
		/// @brief Gets the Y offset
		/// @return Y offset in pixels.
		inline float getYOffset() const { return m_YOffset; }

		/// @brief Converts the MouseScrolledEvent to a string.
		/// @return String containing the scroll offset.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << m_XOffset << ", " << m_YOffset << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		/// @brief Scroll offset.
		float m_XOffset, m_YOffset;

	};




	/// @brief Mouse scrolled event, created when a mouse button is clicked.
	///
	/// Holds a mouse button code.
	/// @see MouseButtonCodes.h
	class CHROMA_API MouseButtonEvent : public Event
	{
	public:
		/// @brief Get the mouse button that was clicked.
		/// @return Mouse Button Code.
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	protected:
		/// @brief Constructs a MouseButtonEvent.
		/// @param button Mouse button pressed.
		MouseButtonEvent(int button)
			: m_Button(button)
		{
		}
		/// @brief Button pressed.
		int m_Button;
	};



	/// @brief Mouse scrolled event, created when a mouse button is pressed down.
	///
	/// Holds a mouse button code.
	/// @see MouseButtonCodes.h
	class CHROMA_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		/// @brief Constructs a MouseButtonPressedEvent
		/// @param button Mouse button code.
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button)
		{
		}

		/// @brief Converts the MouseButtonPressedEvent to a string.
		/// @return String containing the button code.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: (" << m_Button << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)

	};



	/// @brief Mouse scrolled event, created when a mouse button is released.
	///
	/// Holds a mouse button code.
	/// @see MouseButtonCodes.h
	class CHROMA_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		/// @brief Constructs a MouseButtonReleasedEvent
		/// @param button Mouse button code.
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button)
		{
		}

		/// @brief Converts the MouseButtonReleasedEvent to a string.
		/// @return String containing the button code.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: (" << m_Button << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)

	};


}