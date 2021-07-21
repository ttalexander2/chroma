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
#include <Chroma/Components/SpriteRenderer.h>
#include "Style.h"


namespace Polychrome
{

	std::vector<Chroma::Ref<Chroma::Texture2D>> testAseprite;

	EditorApp::EditorApp()
		: Application("Polychrome Editor", 1920U, 1080U), m_CameraController(1920.0f / 1080.0f)
	{

#ifdef CHROMA_PLATFORM_WINDOWS
		glfwSetDropCallback((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), FileDrop::HandleFileDrop);
#endif
		
		Chroma::Scene* scene = new Chroma::Scene();

		
		
		Chroma::EntityRef entity = scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();


		auto entity4 = scene->NewEntity();

		entity.AddComponent<Chroma::SpriteRenderer>();
		auto sprite = entity.GetComponent<Chroma::SpriteRenderer>();


		Chroma::EntityRef e2 = scene->NewEntity();
		Chroma::ComponentRef aaaa = e2.GetComponent<Chroma::BoxCollider2D>();

		scene->AddComponent<Chroma::CircleCollider2D>(e2);
		scene->AddComponent<Chroma::BoxCollider2D>(e2);

		std::string prefab = e2.ToPrefab();
		std::ofstream fout("test.prefab");
		fout << prefab;


		//scene->GetAllComponents(entity);


		CHROMA_INFO("");

		this->m_ActiveScene = scene;

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

		m_ActiveScene->EarlyInit();
		m_ActiveScene->Init();
		m_ActiveScene->LateInit();


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

		//ase_t* ase = cute_aseprite_load_from_file("assets/textures/test.ase", NULL);
		//auto texture = Chroma::Texture2D::Create(ase->w, ase->h);
		//texture->SetData(ase->frames->pixels, sizeof(ase->frames->pixels)/sizeof(ase->frames->pixels[0]));

		Chroma::Aseprite test("assets/textures/test.ase");
		CHROMA_INFO("IMAGE {0}; [{1}, {2}]", "assets/textures/test.ase", test.frames[0].image.Width, test.frames[0].image.Height);
		test.frames[0].image.SavePNG("test2.png");

		Chroma::Color* data = new Chroma::Color[test.width * test.height];

		testAseprite.reserve(test.frames.size());

		for (int i = 0; i < test.frames.size(); i++)
		{

			testAseprite.push_back(Chroma::Texture2D::Create(test.width, test.height));

			test.frames[i].image.FlipVertically();
			test.frames[i].image.GetData(data);
			testAseprite[i]->SetData(data, sizeof(Chroma::Color) * test.width * test.height);
		}

		delete[] data;

	}

	void EditorApp::Update(Chroma::Time time)
	{
		//CHROMA_ERROR("{0}", Application::Get().m_ActiveScene);

		m_ActiveScene->EarlyUpdate(time);
		m_ActiveScene->Update(time);
		m_ActiveScene->LateUpdate(time);

		CHROMA_PROFILE_FUNCTION();
		// Update
		{
			CHROMA_PROFILE_SCOPE("CameraController OnUpdate");
			m_CameraController.OnUpdate(time);
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

		m_ActiveScene->EarlyDraw(time);
		m_ActiveScene->Draw(time);
		m_ActiveScene->LateDraw(time);

		static float timeSince = 0;
		static int frame = 0;
		timeSince += time;
		if (timeSince > 0.3)
		{
			timeSince = 0;
			frame++;
		}

		int i = frame % testAseprite.size();
		Chroma::Renderer2D::DrawQuad({ 0,0 }, { 64,64 }, testAseprite[i]);

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

					Chroma::Renderer2D::DrawQuad({ x, y }, { 1, 1 }, m_SquareColor);
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

		style.WindowMinSize.x = 100.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		}

		style.WindowMinSize.x = minWinSizeX;

		int prevFramePaddingY = ImGui::GetStyle().FramePadding.y;
		ImGui::GetStyle().FramePadding.y = 6.0f;

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

		ImGui::EndMainMenuBar();

		ImGui::GetStyle().FramePadding.y = prevFramePaddingY;

		//ImGui::PopStyleVar();

		ImGui::ShowDemoWindow();

		Hierarchy::Draw();
		Inspector::Draw();
		Viewport::Draw(m_Framebuffer);

		
		Chroma::ImGuiDebugMenu::Draw();
		if (ImGui::Begin("Settings"))
		{
			static int selected_style = Config.Style;
			ImGui::SelectStyleCombo("Style", &selected_style, ImGui::ImGuiStyle_Count, nullptr);
		}
		ImGui::End();

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
			if (Chroma::Scene::Deserialize(*out, strStream.str()))
			{
				CurrentScenePath = filepath;
				delete this->m_ActiveScene;
				this->m_ActiveScene = out;
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
			std::string yaml = this->m_ActiveScene->Serialize();
			std::ofstream fout2(filepath);
			fout2 << yaml;
			CurrentScenePath = filepath;
		}
	}

	void EditorApp::SaveScene()
	{
		if (!CurrentScenePath.empty() && std::filesystem::exists(CurrentScenePath))
		{
			std::string yaml = this->m_ActiveScene->Serialize();
			std::ofstream fout2(CurrentScenePath);
			fout2 << yaml;
		}
	}



}

Chroma::Application* Chroma::CreateApplication()
{
	return new Polychrome::EditorApp();
}

