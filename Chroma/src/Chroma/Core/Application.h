#pragma once

#include "chromapch.h"

#include "Core.h"
#include "Chroma/Core/Window.h"
#include "Chroma/Events/ApplicationEvent.h"
#include <glm/glm.hpp>
#include <Chroma/ImGui/ImGuiLayer.h>
#include "Chroma/Core/Time.h"

#include "Chroma/Renderer/Shader.h"
#include "Chroma/Renderer/VertexBuffer.h"
#include "Chroma/Renderer/IndexBuffer.h"
#include "Chroma/Renderer/Pipeline.h"
#include "Chroma/Scene/Scene.h"


namespace Chroma
{
	/// @brief Main class for a Game in the Chroma Engine.
	///
	/// This class handles the update loop for the game, as well as executing all backend events.
	/// Overriding this class will create the entry point for your game.
	/// In order to set the entry point, see the following example:
	/// 
	/// ```{.cpp}
	/// Chroma::Application* Chroma::CreateApplication()
	/// {
	/// 	return new Runtime();
	/// }
	/// ```
	class CHROMA_API Application
	{
	public:

		/// @brief Constructs a new Chroma Engine Application.
		/// @param title Window Title.
		/// @param width Width of the window.
		/// @param height Height of the window.
		Application(const std::string& title = "Chroma Engine", unsigned int width = 1920, unsigned int height = 1080, bool child = false);
		virtual ~Application();

		virtual void BindScripts() {};

		/// @brief Main update loop for the chroma engine application.
		///
		/// This function is automatically called, and should never be called outside the scope of engine entry point.
		void Run();

		/// @brief Processes and propagates application events.
		/// @param e Event to process.
		void ProcessEvents(Event& e);

		/// @brief Initialization phase, called before the main update loop, after EarlyInit() and before LateInit().
		virtual void Init() {};

		/// @brief Update Step. Called once per frame.
		/// @param time Current delta time in seconds.
		virtual void Update(Time time) {};

		/// @brief Draw Step. Called once per frame
		/// @param time Current delta time in seconds.
		virtual void Draw(Time time) {};

		/// @brief ImGui draw Step. Called once per frame, draws all ImGui needed for the
		/// @param time Current delta time in seconds.
		virtual void ImGuiDraw(Time time) {};

		/// @brief Event handling function. All events will call this function.
		/// @param e Current event.
		virtual void OnEvent(Event& e) {};

		virtual void OnDestroy() {};

		/// @brief Get the current window.
		Window& GetWindow() { return *m_Window; }

		/// @brief Get the current instance of the application.
		static Application& Get() { return *s_Instance; }

		/// @brief Get the ImGui Layer.
		/// @return 
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		/// @brief Get the current time.
		/// Alternatively Time can be accessed statically.
		Time GetTime() { return m_Time; }

		inline const bool IsRunning() const { return m_Running; }

		void Stop();
		

	private:


		static Application* s_Instance;

		Time m_Time;
		Time m_VariableTime;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
	};

	// Game entry point, defined in game project
	Application* CreateApplication();
}

