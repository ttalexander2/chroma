#include "Sandbox2D.h"

#include <Chroma.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_Texture = Chroma::Texture2D::Create("assets/textures/megagfilms.png");

	Chroma::FramebufferSpecification fbspec;
	fbspec.Width = 1920;
	fbspec.Height = 1080;
	m_Framebuffer = Chroma::Framebuffer::Create(fbspec);
}

void Sandbox2D::OnUpdate(Chroma::Time ts)
{
	CHROMA_PROFILE_FUNCTION();
	// Update
	{
		CHROMA_PROFILE_SCOPE("CameraController OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	// Render
	{
		CHROMA_PROFILE_SCOPE("Render OnUpdate");
		Chroma::RenderCommand::SetClearColor({ 0.0f, 0.0f , 0.0f , 1.0f });
		Chroma::RenderCommand::Clear();
		Chroma::Renderer2D::ResetStats();
		m_Framebuffer->Bind();

		static float rotation = 0.0f;
		rotation += ts * 5.0f;

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
				Chroma::Renderer2D::DrawQuad({ x, y, 0.0f }, { 0.08f, 0.08f } , m_SquareColor);
			}
		}

		Chroma::Renderer2D::DrawQuad({ 0.2f, -0.6f }, { 0.4f, 0.8f }, m_SquareColor);
		Chroma::Renderer2D::DrawQuad({ -0.4f, -0.3f }, { 0.2f, 0.3f }, m_SquareColor, rotation);

		Chroma::Renderer2D::DrawQuad({ 0.2f, 0.3f, 0.0f }, { 0.4f, 0.4f }, m_Texture);

		Chroma::Renderer2D::EndScene();

		m_Framebuffer->Unbind();

	}
}

void Sandbox2D::OnImGuiRender()
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

	ImGui::Begin("Viewport");

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImGui::GetWindowSize());

	ImGui::End();
}

void Sandbox2D::OnEvent(Chroma::Event& e)
{
	m_CameraController.OnEvent(e);
}
