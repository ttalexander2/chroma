#pragma once

#include "chromapch.h"

#include "Core.h"
#include "Chroma/Core/Window.h"
#include "Chroma/Events/ApplicationEvent.h"
#include "Chroma/Core/Layer.h"
#include "Chroma/Core/LayerStack.h"
#include <glm/glm.hpp>
#include <Chroma/ImGui/ImGuiLayer.h>

#include "Chroma/Renderer/Shader.h"
#include <Chroma/Renderer/Buffer.h>
#include <Chroma/Renderer/VertexArray.h>

namespace Chroma
{
	class CHROMA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& GetInstance() { return *s_Instance; }

		inline ImGuiLayer& GetImGuiLayer() { return *m_ImGuiLayer; }

	private:

		static Application* s_Instance;

		float m_LastFrameTime = 0.0f;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
	};

	// Game entry point, defined in game project
	Application* CreateApplication();
}

