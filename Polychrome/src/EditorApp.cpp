#include "EditorApp.h"

#include "Viewport.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDrop.h"

#include <Chroma.h>
#include <Chroma/Core/EntryPoint.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <fmod_studio.hpp>
#include <random>
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/CircleCollider2D.h>
#include <Chroma/Components/BoxCollider2D.h>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/Utilities/FileDialogs.h>
#include <filesystem>
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include "Fonts/Roboto.cpp"
#include <Chroma/Images/Aseprite.h>
#include "Style.h"
#include <Chroma/Components/Transform.h>
#include <Chroma/Utilities/FileWatcher.h>
#include "readerwriterqueue.h"
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Components/LuaScript.h>
#include "TextEdit.h"
#include <Chroma/Systems/SpriteRendererSystem.h>


namespace Polychrome
{

	std::vector<Chroma::Ref<Chroma::Texture2D>> testAseprite;


	moodycamel::ReaderWriterQueue<std::function<void()>> file_queue;

	std::thread file_watcher_thread;
	std::atomic_bool file_watcher_thread_running;

	Chroma::Scene* EditorApp::CurrentScene = nullptr;

	bool EditorApp::SceneRunning = false;
	bool EditorApp::ScenePaused = false;
	bool EditorApp::PreviewSprites = true;

	EditorApp::EditorApp()
		: Application("Polychrome Editor", 1920U, 1080U), m_CameraController(1920.0f / 1080.0f)
	{

#ifdef CHROMA_PLATFORM_WINDOWS
		glfwSetDropCallback((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), FileDrop::HandleFileDrop);
#endif
		
		Chroma::Scene* scene = new Chroma::Scene();

		
		
		Chroma::Entity entity = scene->NewEntity();


		auto entity4 = scene->NewEntity();
		entity.AddComponent<Chroma::SpriteRenderer>();
		auto &sprite = entity.GetComponent<Chroma::SpriteRenderer>();
		entity.AddComponent<Chroma::LuaScript>();
		entity.CreateChild();
		entity.CreateChild();
		entity.CreateChild();

		((Chroma::Transform*)entity.GetComponent("Transform"))->Position.x = 2;





		Chroma::Entity e2 = scene->NewEntity();
		Chroma::BoxCollider2D& aaaa = e2.GetComponent<Chroma::BoxCollider2D>();

		//scene->AddComponent<Chroma::CircleCollider2D>(e2);
		//scene->AddComponent<Chroma::BoxCollider2D>(e2);

		std::string prefab = e2.ToPrefab();
		std::ofstream fout("test.prefab");
		fout << prefab;


		//scene->GetAllComponents(entity);


		//CHROMA_INFO("");

		EditorApp::CurrentScene = scene;


		// This needs to be replaced
		for (auto& file : std::filesystem::recursive_directory_iterator(".\\assets"))
		{
			if (file.is_regular_file())
			{
				std::string extension = file.path().extension().string();
				if (extension == ".ase" || extension == ".png" || extension == ".jpg")
				{
					CHROMA_CORE_INFO("{}", file.path().string());
					Chroma::AssetManager::LoadSprite(file.path().string());
				}
			}
		}

		
		file_watcher_thread_running.store(true);

		file_watcher_thread = std::thread([] {
			FileWatcher fw{ ".\\assets", std::chrono::milliseconds(3000) };
			fw.start(file_watcher_thread_running, [](std::string path_to_watch, FileStatus status) -> void {
				// Process only regular files, all other file types are ignored
				if (!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased)
				{
					return;
				}

				switch (status)
				{
				case FileStatus::created:
					file_queue.enqueue([path_to_watch]() {
						CHROMA_CORE_TRACE("File Created: {}", path_to_watch);
						std::string extension = std::filesystem::path(path_to_watch).extension().string();
						if (extension == ".ase" || extension == ".png" || extension == ".jpg")
						{
							Chroma::AssetManager::LoadSprite(path_to_watch);
						}
					});
					break;
				case FileStatus::modified:
					file_queue.enqueue([path_to_watch]() {
						CHROMA_CORE_TRACE("File Modified: {}", path_to_watch);
						std::string extension = std::filesystem::path(path_to_watch).extension().string();
						if (extension == ".ase" || extension == ".png" || extension == ".jpg")
						{
							if (Chroma::AssetManager::HasSprite(path_to_watch))
							{
								Chroma::AssetManager::GetSprite(path_to_watch)->Reload();
							}
							else
							{
								Chroma::AssetManager::LoadSprite(path_to_watch);
							}
							
						}
					});
					break;
				case FileStatus::erased:
					file_queue.enqueue([path_to_watch]() {
						CHROMA_CORE_TRACE("File Erased: {}", path_to_watch);
						std::string extension = std::filesystem::path(path_to_watch).extension().string();
						if (extension == ".ase" || extension == ".png" || extension == ".jpg")
						{
							
						}
					});
					
					break;
				default:
					CHROMA_CORE_TRACE("Error! Unknown file status.\n");
				}
			});
		});

		/*
		for (int i = 0; i < 500; i++)
		{
			auto aaaa = EditorApp::CurrentScene->NewEntity();
			aaaa.AddComponent<Chroma::LuaScript>();
			aaaa.AddComponent<Chroma::SpriteRenderer>().SetSprite(".\\assets\\textures\\test.ase");
		}
		*/



		scene->PreLoad();
		scene->Load();
		scene->PostLoad();


		

	}

	void EditorApp::Init()
	{
		m_Texture = Chroma::Texture2D::Create("assets/textures/megagfilms.png");

		Chroma::FramebufferInfo fbspec;
		fbspec.Width = 1920;
		fbspec.Height = 1080;
		m_Framebuffer = Chroma::Framebuffer::Create(fbspec);

		Chroma::Audio::LoadBank("assets/fmod/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
		Chroma::Audio::LoadBank("assets/fmod/Desktop/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);


		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 14.0f);
		
		

		ImFontConfig config;
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
		ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 16.0f, &config, icon_ranges);
		io.FontDefault = font;


		ImGui::GetStyle().FrameRounding = 1;

		Config.Style = (int)ImGui::ImGuiStylePreset::Cherry;

		TextEdit::Init();

		

	}


	void EditorApp::Update(Chroma::Time time)
	{

		if (EditorApp::SceneRunning && !EditorApp::ScenePaused)
		{
			EditorApp::CurrentScene->EarlyUpdate(time);
			EditorApp::CurrentScene->Update(time);
			EditorApp::CurrentScene->LateUpdate(time);
		}
		else if (!EditorApp::SceneRunning && !EditorApp::ScenePaused)
		{
			sprite_system.m_Scene = EditorApp::CurrentScene;
			sprite_system.LateUpdate(time);
		}




		CHROMA_PROFILE_FUNCTION();
		// Update
		{
			CHROMA_PROFILE_SCOPE("CameraController OnUpdate");
			m_CameraController.OnUpdate(time);
		}

		std::function<void()> func;
		bool success;
		while (success = file_queue.try_dequeue(func))
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
		Chroma::RenderCommand::Clear();
		Chroma::Renderer2D::ResetStats();
		m_Framebuffer->Bind();

		static float rotation = 0.0f;
		rotation += time * 20.0f;

		Chroma::RenderCommand::SetClearColor({ 0.0f, 0.0f , 0.0f , 1.0f });
		Chroma::RenderCommand::Clear();



		Chroma::Renderer2D::BeginScene(m_CameraController.GetCamera());

		EditorApp::CurrentScene->PreDraw(time);
		EditorApp::CurrentScene->Draw(time);
		EditorApp::CurrentScene->PostDraw(time);

#if 0
		
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0, 256);


		{
			CHROMA_PROFILE_SCOPE("Draw all the squares");
			int numQuads = 0;
			for (int y = -50; y < 50; y++)
			{
				for (int x = -50; x < 50; x++)
				{
					numQuads++;

					Chroma::Renderer2D::DrawQuad({ x, y }, { 0.9, 0.9 }, m_SquareColor);
				}
			}
		}


		Chroma::Renderer2D::DrawQuad({ 0.2f, -0.6f }, { 0.4f, 0.8f }, m_SquareColor);
		Chroma::Renderer2D::DrawQuad({ -0.4f, -0.3f }, { 0.2f, 0.3f }, m_SquareColor, rotation);

		Chroma::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1, 1 }, m_Texture);
#endif

		Chroma::Renderer2D::EndScene();

		m_Framebuffer->Unbind();
	}

	static bool first = false;

	void EditorApp::ImGuiDraw(Chroma::Time time)
	{
		if (!first)
		{
			ImGui::ResetStyle(ImGui::ImGuiStylePreset::Cherry, ImGui::GetStyle());

			first = true;
		}
			

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
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
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
				SaveScene();

			if (ImGui::MenuItem("Save As...##MAIN_MENU_BAR", "Ctrl+Shift+S"))
				SaveSceneAs();
			
			ImGui::MenuItem("Close##MAIN_MENU_BAR", "Alt + F4");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("Undo##MAIN_MENU_BAR", "Ctrl+Z"))
			{
				//UNDO
			}
			if (ImGui::MenuItem("Redo##MAIN_MENU_BAR", "Ctrl+Y"))
			{
				//REDO
			}

			ImGui::EndMenu();
		}

		static bool SettingsWindowOpen = false;

		if (ImGui::BeginMenu("Window##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("Hierarchy##MAIN_MENU_BAR", "", &Hierarchy::Open))
			{
				//Hierarchy::Open = !Hierarchy::Open;
			}
			if (ImGui::MenuItem("Inspector##MAIN_MENU_BAR", "", &Inspector::Open))
			{
				//Inspector::Open = !Inspector::Open;
			}
			if (ImGui::MenuItem("Viewport##MAIN_MENU_BAR", "", &Viewport::Open))
			{
				//Viewport::Open = !Viewport::Open;
			}
			if (ImGui::MenuItem("Settings##MAIN_MENU_BAR", "", &SettingsWindowOpen))
			{
				//SettingsWindowOpen = !SettingsWindowOpen;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		ImGui::GetStyle().FramePadding.y = prevFramePaddingY;

		//ImGui::PopStyleVar();

		ImGui::ShowDemoWindow();

		Hierarchy::Draw();
		Inspector::Draw();
		Viewport::Draw(m_Framebuffer);
		TextEdit::Draw();

		
		//Chroma::ImGuiDebugMenu::Draw();

		if (SettingsWindowOpen)
		{
			if (ImGui::Begin("Settings", &SettingsWindowOpen))
			{
				static int selected_style = Config.Style;
				ImGui::SelectStyleCombo("Style", &selected_style, ImGui::ImGuiStyle_Count, nullptr);
			}
			ImGui::End();
		}


		ImGui::End();


	}

	void EditorApp::OnEvent(Chroma::Event& e)
	{
		m_CameraController.OnEvent(e);
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
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(filepath);
			fout2 << yaml;
			CurrentScenePath = filepath;
		}
	}

	void EditorApp::SaveScene()
	{
		if (!CurrentScenePath.empty() && std::filesystem::exists(CurrentScenePath))
		{
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(CurrentScenePath);
			fout2 << yaml;
		}
	}



	EditorApp::~EditorApp()
	{
		delete EditorApp::CurrentScene;
		file_watcher_thread_running.store(false);
		file_watcher_thread.join();
	}



}

Chroma::Application* Chroma::CreateApplication()
{
	return new Polychrome::EditorApp();
}

