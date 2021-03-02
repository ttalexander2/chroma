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
	m_Texture = Chroma::Texture2D::Create("assets/textures/Catalyst Logo.bmp");
}

void Sandbox2D::OnUpdate(Chroma::Timestep ts)
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
		Chroma::RenderCommand::SetClearColor({ 0.1f, 0.1f , 0.1f , 0.0f });
		Chroma::RenderCommand::Clear();

		Chroma::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Chroma::Renderer2D::DrawQuad({ 0.2f, -0.6f }, { 0.4f, 0.8f }, m_SquareColor);
		Chroma::Renderer2D::DrawQuad({ -0.4f, -0.3f }, { 0.2f, 0.3f }, m_SquareColor);
		Chroma::Renderer2D::DrawQuad({ 0.2f, 0.3f }, { 0.4f, 0.4f }, m_Texture);

		Chroma::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
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
}

void Sandbox2D::OnEvent(Chroma::Event& e)
{
	m_CameraController.OnEvent(e);
}
