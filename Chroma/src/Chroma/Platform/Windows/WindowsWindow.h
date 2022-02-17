#pragma once

#include "Chroma/Core/Window.h"
#include "Chroma/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>


namespace Chroma
{
	/// @brief Windows platform dependent implementation of abstract Window.
	class WindowsWindow : public Window
	{
	public:
		/// @brief Construct a windows window.
		/// @param props Properties for the window (width, height, title).
		WindowsWindow(const WindowProps& props);

		/// @brief Destroy the window.
		virtual ~WindowsWindow();

		/// @brief Polls window events and swap buffers. Called once per frame.
		void OnUpdate() override;

		/// @brief Get window width.
		/// @return Width in pixels.
		inline unsigned int GetWidth() const override { return m_Data.Width; }

		/// @brief Get window height.
		/// @return Height in pixels.
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		//Window attributes

		/// @brief Set the event callback function.
		/// @param callback Function to be called on window event.
		inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		/// @brief Sets window vsync.
		/// @param enabled Value to set.
		void SetVSync(bool enabled) override;

		/// @brief Check if VSync is enabled.
		/// @return Returns true if VSync is enabled.
		bool IsVSync() const override;

		/// @brief Gets the native window.
		/// @return Returns pointer to native window handle.
		inline virtual void* GetNativeWindow() const { return m_Window; }

		inline virtual GraphicsContext* GetGraphicsContext() const override { return m_Context; }

	private:
		/// @brief Initialize the window.
		/// @param props Properties of the window.
		virtual void Init(const WindowProps& props);
		/// @brief Shutdown the window.
		virtual void Shutdown();
	private:
		/// @brief Pointer to the window.
		GLFWwindow* m_Window;
		/// @brief Graphics context.
		GraphicsContext* m_Context;

		/// @brief Window information.
		struct WindowData
		{
			/// @brief Window titlebar title.
			std::string Title;
			/// @brief Size of the window.
			unsigned int Width, Height;
			/// @brief Whether the window has VSync
			bool VSync;
			/// @brief Callback function for window events.
			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}


