#include "EditorApp.h"

#include "Viewport.h"

#include <Chroma.h>
#include <Chroma/Core/EntryPoint.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <fmod_studio.hpp>

namespace Polychrome
{

	EditorApp::EditorApp()
		: Application("Polychrome Editor", 1920U, 1080U), m_CameraController(1920.0f / 1080.0f)
	{


		struct Component_A
		{
			int x = 0;
		};

		struct Component_B
		{
			int x = 0;
		};

		struct Component_C
		{
			int x = 0;
		};

		Chroma::Scene scene;
		
		Chroma::Entity entity = scene.NewEntity();
		Chroma::Component<Component_A> comp = scene.AddComponent<Component_A>(entity);

		comp = scene.GetComponent<Component_A>(entity);
		comp->x = 2;
		Chroma::Component<Component_B> comp2 = scene.AddComponent<Component_B>(entity);
		comp2->x = 7;

		CHROMA_INFO("ENTITY HAS COMPONENT_A: {0}", entity.HasComponent<Component_A>() ? "True" : "False");
		CHROMA_INFO("ENTITY HAS COMPONENT_C: {0}", entity.HasComponent<Component_C>() ? "True" : "False");

		auto entity4 = scene.NewEntity();

		scene.AddComponent<Component_A>(entity4);
		scene.AddComponent<Component_A>(entity4);
		scene.AddComponent<Component_B>(entity4);
		scene.AddComponent<Component_B>(entity4);
		scene.AddComponent<Component_C>(entity4);
		scene.AddComponent<Component_C>(entity4);

		scene.AddComponent<Component_A>(entity);
		scene.AddComponent<Component_A>(entity);
		scene.AddComponent<Component_A>(entity);
		scene.AddComponent<Component_A>(entity);

		auto components = scene.GetComponents<Component_A>(entity);
		for (Chroma::Component<Component_A> component : components)
		{
			CHROMA_INFO("{0}\n{1}", entity, component);
		}

		CHROMA_INFO("");

		for (Chroma::Component<Component_A> component : components)
		{
			component->x++;
			CHROMA_INFO("{0}->A->x: {1}", entity, component->x);
		}

		for (Chroma::Entity ent : scene.View<Component_A, Component_B>())
		{
			auto a = ent.GetComponent<Component_A>();
			auto b = ent.GetComponent<Component_B>();
			Chroma::Entity q = a.GetScene()->GetEntity(a.GetEntityID());
			CHROMA_INFO("ENTITY [{0}]: a->{1}, b->{2}", ent.GetID(), a->x, b->x);
		}

		Chroma::Entity e2 = scene.NewEntity();

		scene.AddComponent<Component_A>(e2);
		scene.AddComponent<Component_B>(e2);

		for (Chroma::Entity id : scene.View<Component_A, Component_B>())
		{
			//auto a = scene.GetComponent<Component_A>(id);
			//auto b = scene.GetComponent<Component_B>(id);
			//CHROMA_INFO("ENTITY [{0}]: a->{1}, b->{2}", id, a->x, b->x);
		}


		CHROMA_INFO("");

		scene.DestroyEntity(entity);

		this->m_ActiveScene = CreateRef<Chroma::Scene>(scene);

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
		rotation += time * 5.0f;

		Chroma::RenderCommand::SetClearColor({ 0.0f, 0.0f , 0.0f , 1.0f });
		Chroma::RenderCommand::Clear();



		Chroma::Renderer2D::BeginScene(m_CameraController.GetCamera());


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

		Chroma::Renderer2D::DrawQuad({ 0.2f, 0.3f }, { 0.4f, 0.4f }, m_Texture);

		Chroma::Renderer2D::EndScene();

		m_Framebuffer->Unbind();
	}

	void EditorApp::ImGuiDraw(Chroma::Time time)
	{
		ImGui::Begin("Settings");

		ImGui::Text("Renderer2D Stats:");
		auto stats = Chroma::Renderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Separator();
		ImGui::Text("Time: %.5f", this->GetTime());
		ImGui::Text("FPS (no VSync): %.5f", this->GetTime()*60.0*1000);
		ImGui::Separator();

		ImGui::ColorPicker4("Square Color", glm::value_ptr(m_SquareColor));

		for (auto& result : m_ProfileResults)
		{
			char label[50];
			strcpy(label, result.Name);
			strcat(label, "  %.5fms");
			ImGui::Text(label, result.Time);
		}

		m_ProfileResults.clear();

		ImGui::End();

		Viewport::Render(m_Framebuffer);


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

