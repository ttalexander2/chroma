#pragma once

#include "chromapch.h"

#include "Core.h"
#include "Chroma/Window.h"
#include "Chroma/Events/ApplicationEvent.h"
#include "Chroma/Layer.h"
#include "Chroma/LayerStack.h"
#include <glm/glm.hpp>
#include <Chroma/ImGui/ImGuiLayer.h>

#include "Chroma/Renderer/Shader.h"

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

		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
	};

	// Game entry point, defined in game project
	Application* CreateApplication();
}

