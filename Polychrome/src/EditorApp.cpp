#include "EditorApp.h"

#include "Viewport.h"

#include <Chroma.h>
#include <Chroma/Core/EntryPoint.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Polychrome
{

	EditorApp::EditorApp()
		: Application("Polychrome Editor", 1920U, 1080U), m_CameraController(1920.0f / 1080.0f)
	{
		struct A
		{
			int x = 0;
		};
		struct B
		{
			int x = 0;
		};
		struct C
		{
			int x = 0;
		};

		Chroma::Scene s;
		
		Chroma::EntityID entity = s.NewEntity();
		Chroma::Component<A> comp = s.AddComponent<A>(entity);
		comp = s.GetComponent<A>(entity);
		comp->x = 2;
		Chroma::Component<B> comp2 = s.AddComponent<B>(entity);
		comp2->x = 7;

		s.AddComponent<A>(entity);
		s.AddComponent<A>(entity);
		s.AddComponent<A>(entity);
		s.AddComponent<A>(entity);

		auto components = s.GetComponents<A>(entity);
		for (Chroma::Component<A> component : components)
		{
			CHROMA_INFO("{0}->A->x: {1}", entity, component->x);
		}

		CHROMA_INFO("");

		for (Chroma::Component<A> component : components)
		{
			component->x++;
			CHROMA_INFO("{0}->A->x: {1}", entity, component->x);
		}

		for (EntityID id : s.View<A, B>())

		s.DestroyEntity(entity);

	}

	void EditorApp::Initialize()
	{
		m_Texture = Chroma::Texture2D::Create("assets/textures/megagfilms.png");

		Chroma::FramebufferInfo fbspec;
		fbspec.Width = 1920;
		fbspec.Height = 1080;
		m_Framebuffer = Chroma::Framebuffer::Create(fbspec);
	}

	void EditorApp::Update(Chroma::Time time)
	{
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
				Chroma::Renderer2D::DrawQuad({ x, y, 0.0f }, { 0.08f, 0.08f }, m_SquareColor);
			}
		}

		Chroma::Renderer2D::DrawQuad({ 0.2f, -0.6f }, { 0.4f, 0.8f }, m_SquareColor);
		Chroma::Renderer2D::DrawQuad({ -0.4f, -0.3f }, { 0.2f, 0.3f }, m_SquareColor, rotation);

		Chroma::Renderer2D::DrawQuad({ 0.2f, 0.3f, 0.0f }, { 0.4f, 0.4f }, m_Texture);

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

