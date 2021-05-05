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

namespace Polychrome
{

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


		Chroma::EntityRef e2 = scene->NewEntity();

		scene->AddComponent<Chroma::CircleCollider2D>(e2);
		scene->AddComponent<Chroma::BoxCollider2D>(e2);
		auto audio = scene->AddComponent<Chroma::AudioSource>(e2);

		audio->Event = "event:/Music/Test";
		audio->Volume = 0.8f;

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

	}

	void EditorApp::Update(Chroma::Time time)
	{
		//CHROMA_ERROR("{0}", Application::Get().m_ActiveScene);

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

					Chroma::Renderer2D::DrawQuad({ x, y }, { 0.8f, 0.8f }, m_SquareColor);
				}
			}
		}


		Chroma::Renderer2D::DrawQuad({ 0.2f, -0.6f }, { 0.4f, 0.8f }, m_SquareColor);
		Chroma::Renderer2D::DrawQuad({ -0.4f, -0.3f }, { 0.2f, 0.3f }, m_SquareColor, rotation);

		Chroma::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1, 1 }, m_Texture);

		Chroma::Renderer2D::EndScene();

		m_Framebuffer->Unbind();
	}

	void EditorApp::ImGuiDraw(Chroma::Time time)
	{

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

		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		}

		style.WindowMinSize.x = minWinSizeX;

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File##MAIN_MENU_BAR"))
		{
			ImGui::MenuItem("New##MAIN_MENU_BAR");
			if (ImGui::MenuItem("Open...##MAIN_MENU_BAR", "Ctrl + O"))
			{
				std::ifstream stream("test.yaml");
				std::stringstream strStream;
				strStream << stream.rdbuf();
				Chroma::Scene* out = new Chroma::Scene();
				if (Chroma::Scene::Deserialize(*out, strStream.str()))
				{
					delete this->m_ActiveScene;
					this->m_ActiveScene = out;
				}
				else
				{
					delete out;
				}
			}
			if (ImGui::MenuItem("Save...##MAIN_MENU_BAR", "Ctrl + S"))
			{
				std::string yaml = this->m_ActiveScene->Serialize();
				std::ofstream fout2("test.yaml");
				fout2 << yaml;
			}
			ImGui::MenuItem("Save As...##MAIN_MENU_BAR");
			ImGui::MenuItem("Close##MAIN_MENU_BAR", "Alt + F4");

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		//ImGui::PopStyleVar();

		ImGui::ShowDemoWindow();

		Hierarchy::Draw();
		Inspector::Draw();
		Viewport::Draw(m_Framebuffer);

		

		ImGui::End();


	}

	void EditorApp::OnEvent(Chroma::Event& e)
	{
		m_CameraController.OnEvent(e);
	}

}

Chroma::Application* Chroma::CreateApplication()
{
	return new Polychrome::EditorApp();
}

