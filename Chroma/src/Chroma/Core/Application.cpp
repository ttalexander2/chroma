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
#include "Chroma/Reflection/Reflection.h"
#include "Chroma/Components/Collider.h"
#include "Chroma/Components/RigidBody.h"

#include <entt.hpp>
#include "Chroma/Utilities/StringHash.h"
#include <any>
#include <Chroma/Assets/FMODBank.h>
#include <Chroma/Components/AnimationPlayer.h>
#include <Chroma/Assets/Font.h>



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

			Reflection::InitializeDataTypes();

			Scene::RegisterComponent<AudioSource>();
			Scene::RegisterComponent<Collider>();
			Scene::RegisterComponent<Camera>();
			Scene::RegisterComponent<CSharpScript>();
			Scene::RegisterComponent<ParticleEmitter>();
			Scene::RegisterComponent<SpriteRenderer>();
			Scene::RegisterComponent<Tag>();
			Scene::RegisterComponent<Transform>();
			Scene::RegisterComponent<AnimationPlayer>();
			Scene::RegisterComponent<RigidBody>();

			AssetManager::Register<Sprite>();
			AssetManager::Register<FMODBank>();
			AssetManager::Register<Font>();
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

		this->Init();

		const int updateFPS = 60;
		const double dt = 1.0 / updateFPS;

		double elapsedTime = 0.0;
		double accumulator = 0.0;


		double currentTime = glfwGetTime();


		while (m_Running)
		{
			//system events

			const double newTime = glfwGetTime();
			double frameTime = newTime - currentTime;

			if (frameTime > 0.25)
			{
				frameTime = 0.25;
			}

			currentTime = newTime;
			accumulator += frameTime;

			//Logic update
			while (accumulator >= dt)
			{
				elapsedTime += dt;
				accumulator -= dt;

				m_Time = dt;
				
				//Fixed Update
				this->Update(dt);

			}
			
			m_VariableTime = frameTime;

			//this->Update(frameTime);
			this->Draw(frameTime);

			m_ImGuiLayer->Begin();

			this->ImGuiDraw(frameTime);

			m_ImGuiLayer->End();

			m_Window->OnUpdate();

			Audio::Update();
		}

		this->OnDestroy();

		m_ImGuiLayer->OnDetach();
		Audio::Shutdown();
	}

}