#pragma once

#include "chromapch.h"

#include "Core.h"
#include "Chroma/Core/Window.h"
#include "Chroma/Events/ApplicationEvent.h"
#include <glm/glm.hpp>
#include <Chroma/ImGui/ImGuiLayer.h>
#include <Chroma/Core/Time.h>

#include "Chroma/Renderer/Shader.h"
#include <Chroma/Renderer/Buffer.h>
#include <Chroma/Renderer/VertexArray.h>

namespace Chroma
{
	class CHROMA_API Application
	{
	public:
		Application(const std::string& title = "Chroma Engine", unsigned int width = 1280, unsigned int height = 720);
		virtual ~Application();

		void Run();

		void ProcessEvents(Event& e);

		virtual void Initialize() = 0;
		virtual void Update(Time time) = 0;
		virtual void Draw(Time time) = 0;
		virtual void ImGuiDraw(Time time) = 0;

		virtual void OnEvent(Event& e) = 0;

		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	private:

		static Application* s_Instance;

		float m_LastFrameTime = 0.0f;

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

