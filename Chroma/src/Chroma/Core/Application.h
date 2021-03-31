#pragma once

#include "chromapch.h"

#include "Core.h"
#include "Chroma/Core/Window.h"
#include "Chroma/Events/ApplicationEvent.h"
#include <glm/glm.hpp>
#include <Chroma/ImGui/ImGuiLayer.h>
#include "Chroma/Core/Time.h"

#include "Chroma/Renderer/Shader.h"
#include "Chroma/Renderer/Buffer.h"
#include "Chroma/Renderer/VertexArray.h"
#include "Chroma/Scene/Scene.h"

namespace Chroma
{
	class CHROMA_API Application
	{
	public:
		Application(const std::string& title = "Chroma Engine", unsigned int width = 1920, unsigned int height = 1080);
		virtual ~Application();

		void Run();

		void ProcessEvents(Event& e);

		virtual void EarlyInit() {};
		virtual void Init() {};
		virtual void LateInit() {};

		virtual void EarlyUpdate(Time time) {};
		virtual void Update(Time time) {};
		virtual void LateUpdate(Time time) {};

		virtual void PreDraw(Time time) {};
		virtual void Draw(Time time) {};
		virtual void PostDraw(Time time) {};

		virtual void ImGuiDraw(Time time) {};

		virtual void OnEvent(Event& e) {};

		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	public:

		Ref<Scene> m_ActiveScene;

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

