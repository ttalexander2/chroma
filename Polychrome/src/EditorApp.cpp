#include "EditorApp.h"

#include "Viewport.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDrop.h"

#include <Chroma.h>
#include <Chroma/Core/EntryPoint.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include <fmod_studio.hpp>
#include <random>
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/CircleCollider.h>
#include <Chroma/Components/BoxCollider.h>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/Utilities/FileDialogs.h>
#include <filesystem>
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include "Fonts/Pinch.cpp"
#include "Fonts/Roboto.cpp"
#include <Chroma/Images/Aseprite.h>
#include "Style.h"
#include <time.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/Utilities/FileWatcher.h>
#include "readerwriterqueue.h"
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Systems/SpriteRendererSystem.h>
#include "AssetBrowser.h"
#include <Chroma/Scripting/MonoScripting.h>
#include "LogWindow.h"	
#include "Project.h"
#include "FileWatcherThread.h"
#include "ComponentDebugGizmos.h"
#include "ErrorWindow.h"
#include "Build.h"
#include "GameSettings.h"
#include "UndoRedo.h"
#include <Chroma/Components/ParticleEmitter.h>
#include <Chroma/Systems/ParticleSystem.h>

#include <mono/metadata/threads.h>
#include <mono/jit/jit.h>

#define CHROMA_DEBUG_LOG
#include <Chroma/Core/Log.h>
#include "thid_party/platform_folders.h"
#include <chrono>
#include "Utilities/GLFWHelpers.h"
#include <Chroma/Components/Tag.h>
#include <mono/metadata/mono-gc.h>
#include <Chroma/Components/CSharpScript.h>
#include <Chroma/Scripting/MonoScripting.h>

#include "Launcher.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <future>


namespace Polychrome
{
	Chroma::Scene* EditorApp::CurrentScene = nullptr;
	std::string EditorApp::CurrentScenePath;

	EditorCamera EditorApp::Camera;

	bool EditorApp::SceneRunning = false;
	bool EditorApp::ScenePaused = false;
	bool EditorApp::PreviewSprites = true;

	ImFont* EditorApp::LargeIcons = nullptr;
	ImFont* EditorApp::SmallIcons = nullptr;
	ImFont* EditorApp::LargeFont = nullptr;
	ImFont* EditorApp::HeaderFont = nullptr;

	GLFWwindow* temp;

	static bool ProjectLoaded = false;

	static bool clearParticles = false;
	static Chroma::EntityID lastParticles;

	std::string EditorApp::InfoMessage;
	EditorApp::MessageSeverity EditorApp::InfoSeverity = EditorApp::MessageSeverity::Info;

	//static std::thread launcher_thread;




	EditorApp::EditorApp()
		:Chroma::Application("Polychrome Editor", 1920U, 1080U)
	{

		glfwHideWindow((GLFWwindow*)this->GetWindow().GetNativeWindow());



#ifdef CHROMA_PLATFORM_WINDOWS
		glfwSetDropCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), FileDrop::HandleFileDrop);
#endif




	}

	void EditorApp::Init()
	{
		m_HighlightShader = Chroma::Shader::Create(".\\assets\\shaders\\Highlight.glsl");

		Chroma::FramebufferSpecification fbspec;
		fbspec.Width = 1920;
		fbspec.Height = 1080;
		fbspec.Attachments = { Chroma::FramebufferTextureFormat::RGBA8, Chroma::FramebufferTextureFormat::RED_INTEGER, Chroma::FramebufferTextureFormat::Depth };
		m_Framebuffer = Chroma::Framebuffer::Create(fbspec);
		m_GuizmoFramebuffer = Chroma::Framebuffer::Create(fbspec);

		// TODO: Load/Unload FMOD banks on play.
		//Chroma::Audio::LoadBank("assets/fmod/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
		//Chroma::Audio::LoadBank("assets/fmod/Desktop/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);

		ImGuiContext* oldCtx = ImGui::GetCurrentContext();

		

		ImGuiIO& io = ImGui::GetIO();

		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 14.f);



		ImFontConfig config;
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
		ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 14.f, &config, icon_ranges);
		io.FontDefault = font;
		LargeIcons = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 128.0f, nullptr, icon_ranges);
		SmallIcons = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 20.f, nullptr, icon_ranges);


		LargeFont = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 64.f);

		HeaderFont = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 20.f);


		ImGui::GetStyle().FrameRounding = 1;

		Config.Style = (int)ImGui::ImGuiStylePreset::Cherry;

		LogWindow::Init();

		Chroma::Input::SetGamepadConnectionCallback([](auto joystick) { CHROMA_CORE_WARN("Joystick Connected!"); });
		Chroma::Input::SetGamepadDisconnectionCallback([](auto joystick) { CHROMA_CORE_WARN("Joystick Disconnected!"); });

		ImGui::ResetStyle(ImGui::ImGuiStylePreset::Cherry, ImGui::GetStyle());

		ImGuiContext* cnew = ImGui::CreateContext();
		ImGui::SetCurrentContext(cnew);

		auto result = std::async([&]() {
			

			ImGui::SetCurrentContext(cnew);

			Launcher* l = new Launcher();

			GLFWwindow* window = glfwCreateWindow(1280, 720, "Chroma", nullptr, nullptr);
			glfwMakeContextCurrent(window);
			glfwSetWindowUserPointer(window, l);

			glfwSetWindowCenter(window);


			
			ImGui_ImplOpenGL3_Init("#version 410");
			ImGui_ImplGlfw_InitForOpenGL(window, false);

			auto& _io = ImGui::GetIO();
			_io.DisplaySize = ImVec2(1280, 720);
			_io.DeltaTime = 1.f / 60.f;


			l->Init();

			
			glClearColor(.6f, .6f, 1.f, 1.f);

			bool setDecoration = false;

			while (!Launcher::Done)
			{

				if (!Launcher::Loading && Launcher::Done)
					break;

				if (!setDecoration && Launcher::Loading)
				{
					glfwSetWindowSize(window, 800, 400);
					glfwSetWindowCenter(window);
					glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
					setDecoration = true;
				}

				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

				double time = glfwGetTime(); // Platform::GetTime
				static double m_LastFrameTime = time;
				float m_Time = time - m_LastFrameTime;
				m_LastFrameTime = time;

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				int x, y;

				glfwGetWindowSize(window, &x, &y);

				l->ImGuiDraw(m_Time, (float)x, (float)y);



				_io.DisplaySize = ImVec2(x,y);

				//Rendering
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwPollEvents();
				glfwSwapBuffers(window);


			}

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext(cnew);

			ImGui::SetCurrentContext(oldCtx);

			glfwDestroyWindow(window);



			delete l;

			Launcher::Stopped.store(true);
		});

		while (!Launcher::Loading)
		{
			if (Launcher::Stopped)
			{
				this->Stop();
				return;
			}
		
		}
		
		while (!Launcher::ProgressLock.try_lock()) {}
		Launcher::ProgressMessage = "Loading...";
		Launcher::ProgressLock.unlock();

		if (Launcher::create)
		{
			while (!Launcher::ProgressLock.try_lock()) {}
			Launcher::ProgressMessage = "Creating Project File...";
			Launcher::ProgressLock.unlock();
			Project::CreateProject(Launcher::project_name, Launcher::path, "\\Scenes\\" + Launcher::starting_scene);
			Chroma::AssetManager::AssetDirectory = Launcher::path + "\\" + Launcher::project_name + "\\Assets";
			while (!Launcher::ProgressLock.try_lock()) {}
			Launcher::ProgressMessage = "Creating Scene...";
			Launcher::ProgressLock.unlock();
			this->CurrentScene = new Chroma::Scene();
			auto camera = this->CurrentScene->NewEntity();
			this->CurrentScene->SetPrimaryCamera(camera.GetID());
			this->CurrentScene->GetComponent<Chroma::Tag>(camera.GetID()).EntityName = "Camera";
			this->CurrentScene->Name = Launcher::starting_scene;
			EditorApp::CurrentScenePath = Chroma::AssetManager::AssetDirectory + "\\Scenes\\" + Launcher::starting_scene + ".chroma";
			while (!Launcher::ProgressLock.try_lock()) {}
			Launcher::ProgressMessage = "Saving Scene...";
			Launcher::ProgressLock.unlock();
			SaveScene();
		}
		else
		{
			while (!Launcher::ProgressLock.try_lock()) {}
			Launcher::ProgressMessage = "Loading Project...";
			Launcher::ProgressLock.unlock();
			Project::LoadProject(Launcher::path + "\\" + Launcher::project_name + ".polychrome");
			EditorApp::CurrentScenePath = Chroma::AssetManager::AssetDirectory + "\\Scenes\\" + Project::StartingScene + ".chroma";
		}


		while (!Launcher::ProgressLock.try_lock()) {}
		Launcher::ProgressMessage = "Constructing Additional Pylons...";
		Launcher::ProgressLock.unlock();

		Polychrome::FileWatcherThread::SetWatch(Chroma::AssetManager::AssetDirectory);

		for (std::filesystem::path path : std::filesystem::recursive_directory_iterator(Chroma::AssetManager::AssetDirectory))
		{
			if (!AssetBrowser::Icons.contains(path))
			{
				AssetBrowser::LoadFileIcon(path);
			}
		}



		Launcher::Done.store(true);
		while (!Launcher::Stopped)
		{
		
		}

		result.wait();

		ImGui::SetCurrentContext(oldCtx);

		glfwMakeContextCurrent((GLFWwindow*)this->GetWindow().GetNativeWindow());

		glfwSetWindowSize((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), 1920, 1080);
		glfwSetWindowCenter((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
		glfwMaximizeWindow((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
		glfwSetWindowAttrib((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowTitle((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), ("Polychrome Editor - " + Project::Name + " - " + Project::StartingScene).c_str());

		glfwShowWindow((GLFWwindow*)this->GetWindow().GetNativeWindow());

		ProjectLoaded = true;

		//Reset glfw callbacks
		glfwSetMouseButtonCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), NULL);
		glfwSetScrollCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), NULL);
		glfwSetKeyCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), NULL);
		glfwSetCharCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), NULL);
		glfwSetMonitorCallback(NULL);
		
		//Set GLFW callbacks
		glfwSetWindowSizeCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				Chroma::WindowResizeEvent evnt(width, height);
				//CHROMA_CORE_WARN("{0}, {1}", width, height);
				data.EventCallback(evnt);
			});

		glfwSetWindowCloseCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				Chroma::WindowCloseEvent evnt;
				data.EventCallback(evnt);
			});

		glfwSetKeyCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					Chroma::KeyPressedEvent evnt(key, 0);
					data.EventCallback(evnt);
					break;
				}
				case GLFW_RELEASE:
				{
					Chroma::KeyReleasedEvent evnt(key);
					break;
				}
				case GLFW_REPEAT:
				{
					Chroma::KeyPressedEvent evnt(key, 1);
					data.EventCallback(evnt);
					break;
				}
				}
			});

		glfwSetMouseButtonCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					Chroma::MouseButtonPressedEvent evnt(button);
					data.EventCallback(evnt);
					break;
				}
				case GLFW_RELEASE:
				{
					Chroma::MouseButtonReleasedEvent evnt(button);
					data.EventCallback(evnt);
					break;
				}
				}
			});

		glfwSetCharCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			Chroma::KeyTypedEvent evnt(keycode);
			data.EventCallback(evnt);

			});

		glfwSetScrollCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				Chroma::MouseScrolledEvent evnt((float)xOffset, (float)yOffset);
				data.EventCallback(evnt);
			});

		glfwSetCursorPosCallback((GLFWwindow*)this->GetWindow().GetNativeWindow(), [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				Chroma::MouseMovedEvent evnt((float)xPos, (float)yPos);
				data.EventCallback(evnt);
			});


		GetWindow().SetEventCallback(CHROMA_BIND_EVENT_FN(Application::ProcessEvents));

		ImGui_ImplOpenGL3_Init("#version 410");
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)this->GetWindow().GetNativeWindow(), true);

		io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   //Enable keyboard controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  //Enable gamepad controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       //Enable docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     //Enable Multi-viewport windows

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui::LoadIniSettingsFromDisk("EditorLayout.ini");




		
	}


	void EditorApp::OnDestroy()
	{
		ImGui::SaveIniSettingsToDisk("EditorLayout.ini");
	}


	void EditorApp::Update(Chroma::Time time)
	{
		if (!ProjectLoaded)
			return;

		if (EditorApp::SceneRunning && !EditorApp::ScenePaused)
		{
			EditorApp::CurrentScene->Update(time);
		}
		else if (!EditorApp::SceneRunning && !EditorApp::ScenePaused)
		{
			sprite_system.m_Scene = EditorApp::CurrentScene;
			sprite_system.LateUpdate(time);

			//CHROMA_CORE_TRACE("Selected: {}, Last: {}, Clear?: {}", Hierarchy::SelectedEntity, lastParticles, clearParticles);

			if (CurrentScene->Registry.valid(lastParticles) && CurrentScene->HasComponent<Chroma::ParticleEmitter>(lastParticles) && clearParticles && lastParticles != Hierarchy::SelectedEntity && lastParticles != Chroma::ENTITY_NULL)
			{
				auto& emitter = CurrentScene->GetComponent<Chroma::ParticleEmitter>(lastParticles);
				for (Chroma::Particle& p : emitter.Particles)
				{
					p.Active = false;
				}
				clearParticles = false;
			}

			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL && CurrentScene->HasComponent<Chroma::ParticleEmitter>(Hierarchy::SelectedEntity))
			{
				auto& emitter = CurrentScene->GetComponent<Chroma::ParticleEmitter>(Hierarchy::SelectedEntity);
				Chroma::ParticleSystem::UpdateEmitter(emitter, CurrentScene->GetTransformAbsolutePosition(Hierarchy::SelectedEntity), time);
				lastParticles = Hierarchy::SelectedEntity;
				clearParticles = true;
			}


		}

		if (Chroma::Input::IsKeyPressed(Chroma::Input::Key::LEFT_BRACKET))
		{
			Camera.viewMatrix = glm::rotate(Camera.viewMatrix, .01f, { 1, 0, 0 });
			Camera.viewProjectionMatrix = Camera.projectionMatrix * Camera.viewMatrix;
		}
		if (Chroma::Input::IsKeyPressed(Chroma::Input::Key::RIGHT_BRACKET))
		{
			Camera.viewMatrix = glm::rotate(Camera.viewMatrix, -.01f, { 1, 0, 0 });
			Camera.viewProjectionMatrix = Camera.projectionMatrix * Camera.viewMatrix;
		}
		

		std::function<void()> func;
		bool success;
		while (success = FileWatcherThread::file_queue.try_dequeue(func))
		{
			if (success)
				func();
			else
				break;
		}
	}


	void EditorApp::Draw(Chroma::Time time)
	{

		CHROMA_PROFILE_SCOPE("Render OnUpdate");
		Chroma::RenderCommand::SetClearColor({ 0.0f, 0.0f , 0.0f , 1.0f });
		Chroma::Renderer2D::Clear();
		Chroma::Renderer2D::ResetStats();

		if (!ProjectLoaded)
			return;

		m_Framebuffer->Bind();

		static float rotation = 0.0f;
		rotation += time * 20.0f;

		Chroma::RenderCommand::SetClearColor({ 0.08f, 0.08f , 0.15f , 1.0f });
		Chroma::Renderer2D::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		//if (EditorApp::SceneRunning)
		//{
		//	Chroma::Renderer2D::Begin(CurrentScene->GetPrimaryCamera());
		//}
		//else
		//{
			Chroma::Renderer2D::Begin(Camera.GetViewProjectionMatrix());
		//}

		

		EditorApp::CurrentScene->Draw(time);

		Chroma::Renderer2D::End();

		m_Framebuffer->Unbind();

		m_GuizmoFramebuffer->Bind();

		Chroma::Renderer2D::Begin(Camera.GetViewProjectionMatrix());

		Chroma::RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 0.f });
		Chroma::Renderer2D::Clear();

		if (!EditorApp::SceneRunning || EditorApp::ScenePaused)
		{

			DrawSelectionMask(time);

			Chroma::Renderer2D::Flush(m_HighlightShader);
			Chroma::Renderer2D::StartBatch();
		}

		if (Viewport::ShouldDrawGrid)
			ComponentDebugGizmos::DrawGrid();
		ComponentDebugGizmos::DrawGizmos();

		Chroma::Renderer2D::End();

		m_GuizmoFramebuffer->Unbind();
	}

	static bool first = false;

	void EditorApp::ImGuiDraw(Chroma::Time time)
	{


		Camera.ImGuiUpdate();
			

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("MAIN_WINDOW", (bool*)0, window_flags);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.FramePadding.x = 4;
		style.FramePadding.y = 3;

		style.WindowMinSize.x = 100.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImVec2 avail = ImGui::GetContentRegionAvail();
			ImGui::DockSpace(dockspace_id, ImVec2(avail.x, avail.y - ImGui::GetTextLineHeight() - style.FramePadding.y * 2), ImGuiDockNodeFlags_None);
		}

		style.WindowMinSize.x = minWinSizeX;

		int prevFramePaddingY = (int)ImGui::GetStyle().FramePadding.y;
		ImGui::GetStyle().FramePadding.y = 4.0f;

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File##MAIN_MENU_BAR"))
		{
			ImGui::MenuItem("New##MAIN_MENU_BAR");
			if (ImGui::MenuItem("Open...##MAIN_MENU_BAR", "Ctrl+O"))
				OpenScene();

			if (ImGui::MenuItem("Save##MAIN_MENU_BAR", "Ctrl+S"))
			{
				SaveScene();
				Project::SaveCurrentProject();
			}
				

			if (ImGui::MenuItem("Save As...##MAIN_MENU_BAR", "Ctrl+Shift+S"))
			{
				SaveSceneAs();
				Project::SaveCurrentProject();
			}
				
			
			ImGui::MenuItem("Close##MAIN_MENU_BAR", "Alt + F4");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("Undo##MAIN_MENU_BAR", "Ctrl+Z", false, UndoRedo::CanUndo()))
			{
				//UNDO
				UndoRedo::Undo();
			}
			if (ImGui::MenuItem("Redo##MAIN_MENU_BAR", "Ctrl+Y", false, UndoRedo::CanRedo()))
			{
				//REDO
				UndoRedo::Redo();
			}

			ImGui::EndMenu();
		}


		if ((ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_CONTROL) || ImGui::IsKeyDown((int)Chroma::Input::Key::RIGHT_CONTROL)) && ImGui::IsKeyPressed((int)Chroma::Input::Key::Z, false) && UndoRedo::CanUndo())
			UndoRedo::Undo();

		if ((ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_CONTROL) || ImGui::IsKeyDown((int)Chroma::Input::Key::RIGHT_CONTROL)) && ImGui::IsKeyPressed((int)Chroma::Input::Key::Y, false) && UndoRedo::CanRedo())
			UndoRedo::Redo();


		if (ImGui::BeginMenu("Object##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("New Entity##MAIN_MENU_BAR"))
			{
				CurrentScene->NewEntity();
			}
			if (ImGui::BeginMenu("New Entity...##MAIN_MENU_BAR"))
			{
				for (auto& component : Chroma::Scene::GetComponentTypes())
				{
					if (component->IsTypeOf<Chroma::Transform>() || component->IsTypeOf<Chroma::Relationship>() || component->IsTypeOf<Chroma::Tag>())
						continue;

					if (ImGui::MenuItem(component->GetTypeName().c_str()))
					{
						Chroma::Entity ent = CurrentScene->NewEntity();
						ent.AddComponent(component->GetTypeName());
					}
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Add a script", Hierarchy::SelectedEntity != Chroma::ENTITY_NULL))
			{
				for (auto& mod : Chroma::MonoScripting::GetModules())
				{
					if (ImGui::MenuItem(mod.c_str()))
					{
						if (CurrentScene->HasComponent<Chroma::CSharpScript>(Hierarchy::SelectedEntity))
						{
							auto child = CurrentScene->NewChild(Hierarchy::SelectedEntity);
							child.GetComponent<Chroma::Tag>().EntityName = mod + " Entity";
							Chroma::CSharpScript& script = child.AddComponent<Chroma::CSharpScript>();
							script.ModuleName = mod;
							Chroma::MonoScripting::InitScriptEntity(child);
						}
						else
						{
							Chroma::CSharpScript& script = CurrentScene->AddComponent<Chroma::CSharpScript>(Hierarchy::SelectedEntity);
							script.ModuleName = mod;
							Chroma::MonoScripting::InitScriptEntity(Chroma::Entity(Hierarchy::SelectedEntity, CurrentScene));
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		static bool PreferencesWindowOpen = false;
		bool OpenBuildWindow = false;

		if (ImGui::BeginMenu("Build##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("Build Game##MAIN_MENU_BAR"))
			{
				OpenBuildWindow = true;
			}
			ImGui::MenuItem("Package Assets##MAIN_MENU_BAR");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window##MAIN_MENU_BAR"))
		{
			ImGui::MenuItem("Hierarchy##MAIN_MENU_BAR", "", &Hierarchy::Open);
			ImGui::MenuItem("Inspector##MAIN_MENU_BAR", "", &Inspector::Open);
			ImGui::MenuItem("Viewport##MAIN_MENU_BAR", "", &Viewport::Open);
			ImGui::MenuItem("Log##MAIN_MENU_BAR", "", &LogWindow::Open);
			ImGui::MenuItem("Errors##MAIN_MENU_BAR", "", &ErrorWindow::Open);
			ImGui::MenuItem("Game Settings##MAIN_MENU_BAR", "", &GameSettings::Open);
			ImGui::MenuItem("Preferences##MAIN_MENU_BAR", "", &PreferencesWindowOpen);


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		ImGui::GetStyle().FramePadding.y = prevFramePaddingY;

		//ImGui::PopStyleVar();

		ImGui::ShowDemoWindow();

		Hierarchy::Draw();
		Inspector::Draw();
		Viewport::Draw(time, m_Framebuffer, m_GuizmoFramebuffer);
		LogWindow::Draw();
		ErrorWindow::Draw();
		AssetBrowser::Draw();
		GameSettings::Draw();

		if (OpenBuildWindow)
		{
			ImGui::OpenPopup("Build##BUILD_WINDOW");
			OpenBuildWindow = false;
		}
		Build::DrawBuildWindow();



		if (ImGui::Begin("Script Engine Debug"))
		{
			float heap = (float)mono_gc_get_heap_size();
			float usage = (float)mono_gc_get_used_size();

			ImGui::Text("GC Heap: %.2fkb / %.2fkb", usage, heap);
			ImGui::Separator();
			

			if (ImGui::TreeNode("Hierarchy Raw "))
			{
				for (auto& mod : Chroma::MonoScripting::GetModuleHierarchy())
				{
					ImGui::TreeNodeEx(mod.c_str(), ImGuiTreeNodeFlags_Leaf);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			ImGui::Separator();

			if (ImGui::TreeNode("Modules "))
			{
				for (auto& mod : Chroma::MonoScripting::GetModules())
				{
					ImGui::TreeNodeEx(mod.c_str(), ImGuiTreeNodeFlags_Leaf);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}



			ImGui::Separator();


			for (auto& [scene, entityMap] : Chroma::MonoScripting::GetEntityInstanceMap())
			{
				ImGui::PushID(scene.ToString().c_str());
				if (ImGui::TreeNode(("Scene " + scene.ToString()).c_str()))
				{
					for (auto& [entityID, entityInstanceData] : entityMap)
					{
						Chroma::Entity entity = Chroma::Entity(entityID, CurrentScene);
						std::string entityName = "Unnamed Entity";
						if (entity.HasComponent<Chroma::Tag>())
							entityName = entity.GetComponent<Chroma::Tag>().EntityName;

						ImGui::PushID(entityName.c_str());
						if (ImGui::TreeNode(entityName.c_str()))
						{
							auto& script = entity.GetComponent<Chroma::CSharpScript>();
							for (auto& [moduleName, fieldMap] : script.ModuleFieldMap)
							{
								ImGui::PushID(moduleName.c_str());
								if (ImGui::TreeNode(moduleName.c_str()))
								{
									for (auto& [fieldName, field] : fieldMap)
									{
										ImGui::PushID(fieldName.c_str());
										if (ImGui::TreeNodeEx(fieldName.c_str(), ImGuiTreeNodeFlags_Leaf))
										{
											ImGui::TreePop();
										}
										ImGui::PopID();
									}
									ImGui::TreePop();
								}
								ImGui::PopID();
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
		ImGui::End();



		ImGui::Begin("Controller");

		//Chroma::Input::GamepadState state = Chroma::Input::GetGamepadState();
		//ImGui::Text(fmt::format("A:				{}", (int)state[Chroma::Input::GamepadButton::A]).c_str());
		//ImGui::Text(fmt::format("B:				{}", (int)state[Chroma::Input::GamepadButton::B]).c_str());
		//ImGui::Text(fmt::format("X:				{}", (int)state[Chroma::Input::GamepadButton::X]).c_str());
		//ImGui::Text(fmt::format("Y:				{}", (int)state[Chroma::Input::GamepadButton::Y]).c_str());
		//ImGui::Text(fmt::format("Left Bumper:	{}", (int)state[Chroma::Input::GamepadButton::LEFT_BUMPER]).c_str());
		//ImGui::Text(fmt::format("Right Bumper:	{}", (int)state[Chroma::Input::GamepadButton::RIGHT_BUMPER]).c_str());
		//ImGui::Text(fmt::format("Back:			{}", (int)state[Chroma::Input::GamepadButton::BACK]).c_str());
		//ImGui::Text(fmt::format("Start:			{}", (int)state[Chroma::Input::GamepadButton::START]).c_str());
		//ImGui::Text(fmt::format("Guide:			{}", (int)state[Chroma::Input::GamepadButton::GUIDE]).c_str());
		//ImGui::Text(fmt::format("Left Thumb:	{}", (int)state[Chroma::Input::GamepadButton::LEFT_THUMB]).c_str());
		//ImGui::Text(fmt::format("Right Thumb:	{}", (int)state[Chroma::Input::GamepadButton::RIGHT_THUMB]).c_str());
		//ImGui::Text(fmt::format("DPAD_UP:		{}", (int)state[Chroma::Input::GamepadButton::DPAD_UP]).c_str());
		//ImGui::Text(fmt::format("DPAD_DOWN:		{}", (int)state[Chroma::Input::GamepadButton::DPAD_DOWN]).c_str());
		//ImGui::Text(fmt::format("DPAD_LEFT:		{}", (int)state[Chroma::Input::GamepadButton::DPAD_LEFT]).c_str());
		//ImGui::Text(fmt::format("DPAD_RIGHT:	{}", (int)state[Chroma::Input::GamepadButton::DPAD_RIGHT]).c_str());
		ImGui::Text(fmt::format("A:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::A)).c_str());
		ImGui::Text(fmt::format("B:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::B)).c_str());
		ImGui::Text(fmt::format("X:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::X)).c_str());
		ImGui::Text(fmt::format("Y:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::Y)).c_str());
		ImGui::Text(fmt::format("Left Bumper:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::LEFT_BUMPER)).c_str());
		ImGui::Text(fmt::format("Right Bumper:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::RIGHT_BUMPER)).c_str());
		ImGui::Text(fmt::format("Back:			{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::BACK)).c_str());
		ImGui::Text(fmt::format("Start:			{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::START)).c_str());
		ImGui::Text(fmt::format("Guide:			{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::GUIDE)).c_str());
		ImGui::Text(fmt::format("Left Thumb:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::LEFT_THUMB)).c_str());
		ImGui::Text(fmt::format("Right Thumb:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::RIGHT_THUMB)).c_str());
		ImGui::Text(fmt::format("DPAD_UP:		{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_UP)).c_str());
		ImGui::Text(fmt::format("DPAD_DOWN:		{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_DOWN)).c_str());
		ImGui::Text(fmt::format("DPAD_LEFT:		{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_LEFT)).c_str());
		ImGui::Text(fmt::format("DPAD_RIGHT:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_RIGHT)).c_str());

		ImGui::End();
		//Chroma::ImGuiDebugMenu::Draw();

		if (PreferencesWindowOpen)
		{
			if (ImGui::Begin("Preferences", &PreferencesWindowOpen))
			{
				static int selected_style = Config.Style;
				ImGui::SelectStyleCombo("Style", &selected_style, ImGui::ImGuiStyle_Count, nullptr);

			}
			ImGui::End();
		}

		if (InfoSeverity == MessageSeverity::Error)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.15f, 1.f));
			ImGui::Text(ICON_FK_EXCLAMATION_CIRCLE);
		}
		else if (InfoSeverity == MessageSeverity::Warning)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.1f, 1.f));
			ImGui::Text(ICON_FK_EXCLAMATION_TRIANGLE);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.3f, 0.8f, 1.f));
			ImGui::Text(ICON_FK_INFO_CIRCLE);
		}
		ImGui::SameLine();
		ImGui::Text(InfoMessage.c_str());
		ImGui::PopStyleColor();







		



		ImGui::End();


	}


	void EditorApp::NewScene()
	{

	}

	void EditorApp::OpenScene()
	{
		std::string filepath = Chroma::FileDialogs::OpenFile("Chroma Scene (*.chroma)\0*.chroma\0");
		if (!filepath.empty())
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;
			strStream << stream.rdbuf();
			Chroma::Scene* out = new Chroma::Scene();
			if (Chroma::Scene::Deserialize(out, strStream.str()))
			{
				CurrentScenePath = filepath;
				delete EditorApp::CurrentScene;
				EditorApp::CurrentScene = out;
				Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
			}
			else
			{
				delete out;
			}
		}
	}
	void EditorApp::SaveSceneAs()
	{
		std::string filepath = Chroma::FileDialogs::SaveFile("Chroma Scene (*.chroma)\0*.chroma\0");
		if (!filepath.empty())
		{
			if (!std::filesystem::path(filepath).has_extension())
				filepath += ".chroma";
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(filepath);
			fout2 << yaml;
			CurrentScenePath = filepath;
		}
	}

	void EditorApp::SaveScene()
	{
		//CHROMA_CORE_TRACE("{}", CurrentScenePath);
		if (!CurrentScenePath.empty() && std::filesystem::exists(CurrentScenePath))
		{
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(CurrentScenePath);
			fout2 << yaml;
		}
		else if (!CurrentScenePath.empty())
		{
			std::filesystem::create_directory(std::filesystem::path(CurrentScenePath).parent_path());
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(CurrentScenePath);
			fout2 << yaml;
		}
	}

	void EditorApp::OnEvent(Chroma::Event& e)
	{
		Chroma::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Chroma::MouseScrolledEvent>(CHROMA_BIND_EVENT_FN(EditorApp::OnMouseScrolled));
	}


	Math::vec2 lerp(Math::vec2 x, Math::vec2 y, float t)
	{
		return x * (1.f - t) + y * t;
	}

	bool EditorApp::OnMouseScrolled(Chroma::MouseScrolledEvent& e)
	{
		if (Viewport::IsViewportFocused() && Viewport::IsViewportHovered())
		{
			float zoom = Camera.GetZoom();
			zoom += e.getYOffset() * zoom / 8.0f;
			Camera.SetZoom(zoom);
			return true;
		}
		return false;
	}


	void EditorApp::DrawSelectionMask(Chroma::Time time)
	{
		static float totalTime = time;
		totalTime += time.GetSeconds();
		float offset = (Math::sin(4.f * totalTime) + 1.f);

		//CHROMA_CORE_INFO("offset: {}", offset);

		if (Hierarchy::SelectedEntity == Chroma::ENTITY_NULL || !EditorApp::CurrentScene->HasComponent<Chroma::SpriteRenderer>(Hierarchy::SelectedEntity))
			return;

		Chroma::Transform& transform = EditorApp::CurrentScene->Registry.get<Chroma::Transform>(Hierarchy::SelectedEntity);
		Chroma::SpriteRenderer& spriteRenderer = EditorApp::CurrentScene->Registry.get<Chroma::SpriteRenderer>(Hierarchy::SelectedEntity);
		Chroma::Relationship& relationship = EditorApp::CurrentScene->Registry.get<Chroma::Relationship>(Hierarchy::SelectedEntity);
		
		const Math::vec2& origin = spriteRenderer.GetSpriteOriginVector();
		
		if (Chroma::AssetManager::HasSprite(spriteRenderer.GetSpriteID()))
		{
			Chroma::Ref<Chroma::Sprite> s = Chroma::AssetManager::GetSprite(spriteRenderer.GetSpriteID());
			int w = s->Frames[spriteRenderer.GetCurrentFrame()].Texture->GetWidth();
			int h = s->Frames[spriteRenderer.GetCurrentFrame()].Texture->GetHeight();
			if (!relationship.IsChild())
			{
				Math::vec2 size = transform.Scale * Math::vec2((float)w, (float)h);
				Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };


				Chroma::Renderer2D::DrawQuad(transform.Position + spriteRenderer.Offset + originAdjustment, transform.Scale * Math::vec2((float)w + offset, (float)h + offset), s->Frames[spriteRenderer.GetCurrentFrame()].Texture, {0.f,1.f,1.f,1.f}, transform.Rotation);
			}
			else
			{
				Math::vec2 pos = transform.Position;
				Math::vec2 scale = transform.Scale;
				Math::vec2 parentPos{ 0,0 };
				float parentRot = 0;
				float rotation = transform.Rotation;
				Chroma::EntityID parent = relationship.Parent;
				while (parent != Chroma::ENTITY_NULL)
				{
					Chroma::Transform& parentTransform = EditorApp::CurrentScene->GetComponent<Chroma::Transform>(parent);
					parentPos += parentTransform.Position;
					scale *= parentTransform.Scale;
					parentRot += parentTransform.Rotation;
					parent = EditorApp::CurrentScene->GetComponent<Chroma::Relationship>(parent).Parent;
				}

				Math::vec2 adjusted = { pos.x * Math::cos(parentRot) - pos.y * Math::sin(parentRot), pos.x * Math::sin(parentRot) + pos.y * Math::cos(parentRot) };
				//CHROMA_CORE_TRACE("Adjusted: [{}, {}]; ParentPos: [{}, {}]; ParentRot: {}", adjusted.x, adjusted.y, parentPos.x, parentPos.y, parentRot);
				Math::vec2 size = scale * Math::vec2((float)w, (float)h);
				Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };

				
				Chroma::Renderer2D::DrawQuad(parentPos + adjusted + spriteRenderer.Offset + originAdjustment, scale * Math::vec2((float)w + offset, (float)h + offset), s->Frames[spriteRenderer.GetCurrentFrame()].Texture, { 0.f,1.f,1.f,1.f }, rotation + parentRot);
			}

		}
	}



	EditorApp::~EditorApp()
	{
		delete EditorApp::CurrentScene;
		if (FileWatcherThread::file_watcher_thread_running)
		{
			FileWatcherThread::file_watcher_thread_running.store(false);
			FileWatcherThread::file_watcher_thread.join();
		}

		//if (launcher_thread.joinable())
		//	launcher_thread.join();
	}
}

Chroma::Application* Chroma::CreateApplication()
{
	return new Polychrome::EditorApp();
}

