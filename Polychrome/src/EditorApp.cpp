#include "EditorApp.h"

#include "Viewport.h"
#include "Hierarchy.h"
#include "Inspector.h"

#include <Chroma.h>
#include <Chroma/Core/EntryPoint.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <fmod_studio.hpp>
#include <random>

namespace Polychrome
{

	EditorApp::EditorApp()
		: Application("Polychrome Editor", 1920U, 1080U), m_CameraController(1920.0f / 1080.0f)
	{
		struct Component_A : Chroma::Component
		{
			int x = 0;

			const std::string Name() const override { return "Component A"; }
		};

		struct Component_B : Chroma::Component
		{
			int x = 0;

			const std::string Name() const override { return "Component B"; }
		};

		struct Component_C : Chroma::Component
		{
			int x = 0;

			const std::string Name() const override { return "Component C"; }
		};
		
		Chroma::Scene* scene = new Chroma::Scene();

		scene->RegisterComponent<Component_A>();
		scene->RegisterComponent<Component_B>();
		scene->RegisterComponent<Component_C>();
		
		Chroma::EntityRef entity = scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();
		scene->NewEntity();

		Chroma::ComponentRef<Component_A> comp = scene->AddComponent<Component_A>(entity);

		comp = scene->GetComponent<Component_A>(entity);
		comp->x = 2;
		Chroma::ComponentRef<Component_B> comp2 = scene->AddComponent<Component_B>(entity);
		comp2->x = 7;

		CHROMA_INFO("ENTITY HAS COMPONENT_A: {0}", entity.HasComponent<Component_A>() ? "True" : "False");
		CHROMA_INFO("ENTITY HAS COMPONENT_C: {0}", entity.HasComponent<Component_C>() ? "True" : "False");


		auto entity4 = scene->NewEntity();


		scene->AddComponent<Component_A>(entity4);
		scene->AddComponent<Component_A>(entity4);
		scene->AddComponent<Component_B>(entity4);
		scene->AddComponent<Component_B>(entity4);
		scene->AddComponent<Component_C>(entity4);
		scene->AddComponent<Component_C>(entity4);

		scene->AddComponent<Component_A>(entity);
		scene->AddComponent<Component_A>(entity);
		scene->AddComponent<Component_A>(entity);
		scene->AddComponent<Component_A>(entity);

		auto components = scene->GetComponents<Component_A>(entity);
		for (Chroma::ComponentRef<Component_A> component : components)
		{
			CHROMA_INFO("{0}\n{1}", entity, component);
		}

		CHROMA_INFO("");

		for (Chroma::ComponentRef<Component_A> component : components)
		{
			component->x++;
			CHROMA_INFO("{0}->A->x: {1}", entity, component->x);
		}

		for (Chroma::EntityRef ent : scene->View<Component_A, Component_B>())
		{
			auto a = ent.GetComponent<Component_A>();
			auto b = ent.GetComponent<Component_B>();
			Chroma::EntityRef q = a.GetScene()->GetEntity(a.GetEntityID());
			CHROMA_INFO("ENTITY [{0}]: a->{1}, b->{2}", ent.GetID(), a->x, b->x);
		}

		Chroma::EntityRef e2 = scene->NewEntity();

		scene->AddComponent<Component_A>(e2);
		scene->AddComponent<Component_B>(e2);

		for (Chroma::EntityRef id : scene->View<Component_A, Component_B>())
		{
			auto a = scene->GetComponent<Component_A>(id);
			auto b = scene->GetComponent<Component_B>(id);
			CHROMA_INFO("ENTITY [{0}]: a->{1}, b->{2}", id, a->x, b->x);
		}


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

		Chroma::Audio::LoadEvent("event:/Music/Test");

	}

	void EditorApp::Update(Chroma::Time time)
	{
		//CHROMA_ERROR("{0}", Application::Get().m_ActiveScene);
		//Chroma::Audio::PlayEventIfStopped("event:/Music/Test");

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_ENTER))
		{
			Chroma::Audio::PlayEventIfStopped("event:/FX/Sigil");
		}


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
			ImGui::MenuItem("Open...##MAIN_MENU_BAR", "Ctrl + O");
			ImGui::MenuItem("Save...##MAIN_MENU_BAR", "Ctrl + S");
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

