#include "chromapch.h"
#include "Application.h"

#include "Chroma/Core/Log.h"
#include "Chroma/Core/Input.h"
#include "Chroma/Core/KeyCodes.h"
#include "Chroma/Core/MouseButtonCodes.h"
#include "Chroma/Renderer/Renderer.h"

#include "Chroma/Core/Timestep.h"
#include <GLFW/glfw3.h>

namespace Chroma
{
	
	Application* Application::s_Instance = nullptr;

	Application::Application() 
	{
		CHROMA_CORE_INFO("Chroma Engine v0.1");
		CHROMA_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = Scope<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}

	Application::~Application() 
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer* layer)
	{
		m_LayerStack.PopOverlay(layer);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(CHROMA_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CHROMA_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			Chroma::Layer* layer = *--it;
			if (!layer->IsEnabled())
			{
				continue;
			}
			layer->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
				{
					if (layer->IsEnabled())
						layer->OnUpdate(timestep);
				}
			}




			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				if (layer->IsEnabled())
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

}