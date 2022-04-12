#include "chromapch.h"
#include "Application.h"

#include "Chroma/Core/Log.h"
#include "Chroma/Core/Input.h"
#include "Chroma/Core/KeyCodes.h"
#include "Chroma/Core/MouseButtonCodes.h"

#include "Chroma/Core/Time.h"
#include "Chroma/Audio/Audio.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "Chroma/Components/Tag.h"
#include "Chroma/Components/Transform.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Components/AudioSource.h"
#include "Chroma/Components/BoxCollider.h"
#include "Chroma/Components/CircleCollider.h"
#include "Chroma/Components/Relationship.h"
#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Scripting/ScriptEngineRegistry.h"
#include "Chroma/Components/CSharpScript.h"
#include "Chroma/Components/Camera.h"
#include "Chroma/Components/ParticleEmitter.h"
#include "Chroma/Renderer/RenderCommand.h"
#include "Chroma/Renderer/Renderer2D.h"
#include "Chroma/Assets/Asset.h"
#include "Chroma/Assets/AssetManager.h"
#include "Chroma/Assets/Sprite.h"

#include <entt.hpp>
#include "Chroma/Utilities/StringHash.h"
#include <any>
#include <Chroma/Assets/FMODBank.h>



namespace Chroma
{
	
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& title, unsigned int width, unsigned int height, bool child)
	{

		if (!child)
		{
			CHROMA_CORE_INFO("Chroma Engine v0.1");
			CHROMA_CORE_ASSERT(!s_Instance, "Application already exists!");
			s_Instance = this;
		}

		WindowProps props;
		props.Title = title;
		props.Width = width;
		props.Height = height;
		m_Window = Scope<Window>(Window::Create(props));
		m_Window->SetEventCallback(CHROMA_BIND_EVENT_FN(Application::ProcessEvents));
		Input::Init();
		RenderCommand::Init();
		Renderer2D::Init();

		m_ImGuiLayer = new ImGuiLayer();

		Audio::Init();
		if (!child)
		{
			MonoScripting::Init("Chroma.Mono.dll");
			ScriptEngineRegistry::RegisterAll();

			Scene::RegisterComponent<AudioSource>();
			Scene::RegisterComponent<BoxCollider>();
			Scene::RegisterComponent<Camera>();
			Scene::RegisterComponent<CircleCollider>();
			Scene::RegisterComponent<CSharpScript>();
			Scene::RegisterComponent<ParticleEmitter>();
			Scene::RegisterComponent<Relationship>();
			Scene::RegisterComponent<SpriteRenderer>();
			Scene::RegisterComponent<Tag>();
			Scene::RegisterComponent<Transform>();

			AssetManager::Register<Sprite>();
			AssetManager::Register<FMODBank>();
		}



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

		RenderCommand::SetViewport(0,0,e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::Stop()
	{
		m_Running = false;
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

		this->OnDestroy();

		m_ImGuiLayer->OnDetach();
		Audio::Shutdown();
	}

}