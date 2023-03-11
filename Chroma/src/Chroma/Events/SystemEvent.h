#pragma once

#include "chromapch.h"
#include "Chroma/Core/Core.h"

namespace Chroma
{
	/// @brief Types of events provided by the engine.
	enum class EventType
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		AppTick,
		AppUpdate,
		AppRender,
		KeyPressed,
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	/// @brief Categories of various events.
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType::##type; }\
		virtual EventType GetEventType() const override { return GetStaticType(); }\
		virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	/// @brief Abstract event.
	class SystemEvent
	{
		friend class EventDispatcher;
	public:
		/// @brief Destroys an event.
		virtual ~SystemEvent() = default;

		/// @brief Bool representing whether an event has been handled.
		bool Handled = false;

		/// @brief Gets the type of this event.
		/// @return EventType enum.
		virtual EventType GetEventType() const = 0;

		/// @brief Gets the name of the event.
		/// @return C string of the event's name.
		virtual const char *GetName() const = 0;

		/// @brief Gets the event category.
		/// @return Bitwise flags from EventCategory.
		virtual int GetCategoryFlags() const = 0;

		/// @brief Converts the event to a std::string.
		/// @return Name of the event.
		virtual std::string ToString() const { return GetName(); }

		/// @brief Checks if the event is of a category.
		/// @param category EventCategory of this event.
		/// @return Returns true if the event is a member of the category.
		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		/// @brief Checks whether the event has been handled.
		bool IsHandled() const { return Handled; }
	};

	/// @brief Class responsible for dispatching events to event listeners.
	class EventDispatcher
	{
		/// @brief Defines an event function. This function should return true if the event should be consumed.
		/// @tparam T Type of event.
		template <typename T>
		using EventFn = std::function<bool(T &)>;

	public:
		/// @brief Constructs a new event dispatcher for an event.
		/// @param event Event to dispatch.
		EventDispatcher(SystemEvent &event) :
			m_Event(event)
		{
		}

		/// @brief Dispatch an event to all consumers.
		/// @tparam T Type of event to dispatch
		/// @param func Callback function to event.
		/// @return Whether the event was handled.
		template <typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*static_cast<T *>(&m_Event));
				return true;
			}
			return false;
		}

	private:
		/// @brief Event for the event dispatcher.
		SystemEvent &m_Event;
	};

	/// @brief Operator to provide the event's string to a std::ostream. This is neccesary for using events with spdlog.
	/// @param os std::ostream reference to provide string to.
	/// @param e Event to convert ot string.
	/// @return returns the new ostream.
	inline std::ostream &operator<<(std::ostream &os, const SystemEvent &e)
	{
		return os << e.ToString();
	}
}
