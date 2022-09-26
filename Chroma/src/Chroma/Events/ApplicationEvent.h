#pragma once

#include "chromapch.h"
#include "Event.h"

namespace Chroma
{
	/// @brief Event created when the window is resized.
	class CHROMA_API WindowResizeEvent : public Event
	{
	public:
		/// @brief Constructs a window resize event.
		/// @param width New width of the window.
		/// @param height New height of the window.
		WindowResizeEvent(unsigned int width, unsigned int height) :
			m_Width(width),
			m_Height(height)
		{
		}

		/// @brief Get the width of the newly resized window.
		/// @return Width in pixels.
		unsigned int GetWidth() const { return m_Width; }

		/// @brief Get the height of the newly resized window.
		/// @return Height in pixels.
		unsigned int GetHeight() const { return m_Height; }

		/// @brief Converts the window resize event information to a string.
		/// @return Window resize as a std::string.
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		/// @brief Width and Height of the window.
		unsigned int m_Width, m_Height;
	};


	/// @brief Event called when the window is closed.
	class CHROMA_API WindowCloseEvent : public Event
	{
	public:
		/// @brief Constructs a window close event.
		WindowCloseEvent()
		{
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/// @brief Event called when the app is updated. Once per frame.
	class CHROMA_API AppUpdateEvent : public Event
	{
	public:
		/// @brief Constructs an app update event.
		AppUpdateEvent()
		{
		}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/// @brief Event called when the app renders a frame.
	class CHROMA_API AppRenderEvent : public Event
	{
	public:
		/// @brief Constructs an app render event.
		AppRenderEvent()
		{
		}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
