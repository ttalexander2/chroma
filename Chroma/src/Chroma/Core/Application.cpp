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
#include <Chroma/Scripting/LuaScripting.h>
#include "Chroma/Scene/ECS.h"
#include "Chroma/Components/Tag.h"
#include "Chroma/Components/Transform.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Components/AudioSource.h"
#include "Chroma/Components/BoxCollider2D.h"
#include "Chroma/Components/CircleCollider2D.h"
#include <Chroma/Components/Relationship.h>
#include <Chroma/Scripting/MonoScripting.h>

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
		Input::Init();

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();

		Audio::Init();

		LuaScripting::Init();
		MonoScripting::Init("Chroma.Mono.dll");

		ECS::RegisterComponent<Tag>();
		ECS::RegisterComponent<Transform>();
		ECS::RegisterComponent<AudioSource>();
		ECS::RegisterComponent<SpriteRenderer>();
		ECS::RegisterComponent<BoxCollider2D>();
		ECS::RegisterComponent<CircleCollider2D>();
		ECS::RegisterComponent<Relationship>();

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
		m_ImGuiLayer->OnAttach();
		m_Window->OnUpdate();

		this->EarlyInit();
		this->Init();
		this->LateInit();


		while (m_Running)
		{
			double time = glfwGetTime(); // Platform::GetTime
			m_Time = time - m_LastFrameTime;
			m_LastFrameTime = time;

			this->EarlyUpdate(m_Time);
			this->Update(m_Time);
			this->LateUpdate(m_Time);

			this->PreDraw(m_Time);
			this->Draw(m_Time);
			this->PostDraw(m_Time);

			m_ImGuiLayer->Begin();

			this->ImGuiDraw(m_Time);

			m_ImGuiLayer->End();

			m_Window->OnUpdate();

			Audio::Update();
		}

		m_ImGuiLayer->OnDetach();
		Audio::Shutdown();
	}

}