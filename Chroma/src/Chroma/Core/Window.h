#pragma once

#include "chromapch.h"

#include "Chroma/Core/Core.h"
#include "Chroma/Events/Event.h"


namespace Chroma
{
	class GraphicsContext;

	/// @brief Properties needed to construct a window.
	struct WindowProps
	{
		/// @brief Titlebar for the window.
		std::string Title;
		/// @brief Width of the window.
		unsigned int Width;
		/// @brief Height of the window.
		unsigned int Height;

		bool Decorated = true;

		/// @brief Constructs a WindowProps object.
		/// @param title Title of the window.
		/// @param width Width of the window.
		/// @param height Height of the window.
		WindowProps(const std::string &title = "Polychrome",
				unsigned int width = 1920,
				unsigned int height = 1080,
				bool decorated = true) :
			Title(title),
			Width(width),
			Height(height),
			Decorated(decorated)
		{
		}
	};

	/// @brief Abstract window class, to be implemented by underlying implementation.
	class CHROMA_API Window
	{
	public:
		/// @brief Defines an event callback function type.
		using EventCallbackFn = std::function<void(Event &)>;

		/// @brief Window destructor.
		virtual ~Window()
		{
		}

		/// @brief Window update function, to be called once per frame to update the window
		virtual void OnUpdate() = 0;

		/// @brief Gets the width of the window.
		/// @return Width of the window in pixels.
		virtual unsigned int GetWidth() const = 0;

		/// @brief Gets the height of the window.
		/// @return Height of the window in pixels.
		virtual unsigned int GetHeight() const = 0;

		//Window attributes

		/// @brief Sets the event callback function, to be executed on any window event.
		/// @param callback Callback function.
		virtual void SetEventCallback(const EventCallbackFn &callback) = 0;

		/// @brief Enable/Disables VSync.
		/// @param enabled Whether VSync should be enabled.
		virtual void SetVSync(bool enabled) = 0;

		/// @brief Checks if VSync is enabled.
		/// @return Returns `true` if VSync is enabled.
		virtual bool IsVSync() const = 0;

		/// @brief Gets the native window implementation object.
		/// @return Returns a void pointer. Casting of this pointer would be dependent on the windowing backend.
		virtual void *GetNativeWindow() const = 0;

		inline virtual GraphicsContext *GetGraphicsContext() const = 0;

		/// @brief Create a new window. The engine should do this for you.
		/// @param props Window properties (name & size).
		/// @return Pointer to the newly created window.
		static Window *Create(const WindowProps &props = WindowProps());
	};
}
