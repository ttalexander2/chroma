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
		
		Chroma::EntityID entity = scene.NewEntity();
		Chroma::Component<Component_A> comp = scene.CreateComponent<Component_A>(entity);
		comp = scene.GetComponent<Component_A>(entity);
		comp->x = 2;
		Chroma::Component<Component_B> comp2 = scene.CreateComponent<Component_B>(entity);
		comp2->x = 7;

		auto entity4 = scene.NewEntity();

		scene.CreateComponent<Component_A>(entity4);
		scene.CreateComponent<Component_A>(entity4);
		scene.CreateComponent<Component_B>(entity4);
		scene.CreateComponent<Component_B>(entity4);
		scene.CreateComponent<Component_C>(entity4);
		scene.CreateComponent<Component_C>(entity4);

		scene.CreateComponent<Component_A>(entity);
		scene.CreateComponent<Component_A>(entity);
		scene.CreateComponent<Component_A>(entity);
		scene.CreateComponent<Component_A>(entity);

		auto components = scene.GetComponents<Component_A>(entity);
		for (Chroma::Component<Component_A> component : components)
		{
			CHROMA_INFO("{0}->A->x: {1}", entity, component->x);
		}

		CHROMA_INFO("");

		for (Chroma::Component<Component_A> component : components)
		{
			component->x++;
			CHROMA_INFO("{0}->A->x: {1}", entity, component->x);
		}

		for (Chroma::EntityID id : scene.View<Component_A, Component_B>())
		{
			auto a = scene.GetComponent<Component_A>(id);
			auto b = scene.GetComponent<Component_B>(id);
			CHROMA_INFO("ENTITY [{0}]: a->{1}, b->{2}", id, a->x, b->x);
		}

		Chroma::EntityID e2 = scene.NewEntity();

		scene.CreateComponent<Component_A>(e2);
		scene.CreateComponent<Component_B>(e2);

		for (Chroma::EntityID id : scene.View<Component_A, Component_B>())
		{
			auto a = scene.GetComponent<Component_A>(id);
			auto b = scene.GetComponent<Component_B>(id);
			CHROMA_INFO("ENTITY [{0}]: a->{1}, b->{2}", id, a->x, b->x);
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
		Chroma::Audio::PlayEvent("event:/Music/Test");
	}

	void EditorApp::Update(Chroma::Time time)
	{

		
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


		int numQuads = 0;

		for (float y = -5.0f; y < 5.0f; y += 0.1f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.1f)
			{
				numQuads++;
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f,  (y + 5.0f) / 10.0, 1.0f };
				Chroma::Renderer2D::DrawQuad({ x, y }, { 0.08f, 0.08f }, m_SquareColor);
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

