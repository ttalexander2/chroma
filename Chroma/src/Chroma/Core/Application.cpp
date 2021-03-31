#include "chromapch.h"
#include "Application.h"

#include "Chroma/Core/Log.h"
#include "Chroma/Core/Input.h"
#include "Chroma/Core/KeyCodes.h"
#include "Chroma/Core/MouseButtonCodes.h"
#include "Chroma/Renderer/Renderer.h"

#include "Chroma/Core/Time.h"
#include "Chroma/Audio/Audio.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

namespace Chroma
{
	
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& title, unsigned int width, unsigned int height)
	{
		CHROMA_CORE_INFO("Chroma Engine v0.1");
		CHROMA_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		WindowProps props;
		props.Title = title;
		props.Width = width;
		props.Height = height;
		m_Window = Scope<Window>(Window::Create(props));
		m_Window->SetEventCallback(CHROMA_BIND_EVENT_FN(Application::ProcessEvents));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();

		Audio::Init();

	}

	Application::~Application() 
	{
	}

	void Application::ProcessEvents(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(CHROMA_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CHROMA_BIND_EVENT_FN(Application::OnWindowResize));

		m_ImGuiLayer->OnEvent(e);
		this->OnEvent(e);
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
		this->EarlyInit();
		this->Init();
		this->LateInit();
		m_ImGuiLayer->OnAttach();

		while (m_Running)
		{
			float time = (float)glfwGetTime(); // Platform::GetTime
			Time timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			this->EarlyUpdate(timestep);
			this->Update(timestep);
			this->LateUpdate(timestep);

			this->PreDraw(timestep);
			this->Draw(timestep);
			this->PostDraw(timestep);

			m_ImGuiLayer->Begin();

			this->ImGuiDraw(timestep);

			m_ImGuiLayer->End();

			m_Window->OnUpdate();

			Audio::Update();
		}

		m_ImGuiLayer->OnDetach();
		Audio::Shutdown();
	}

}