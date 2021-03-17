#pragma once

#include <Chroma.h>

class Sandbox2D : public Chroma::Layer
{
public:
	Sandbox2D();
	~Sandbox2D() = default;

	void OnAttach() override;
	void OnUpdate(Chroma::Time ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Chroma::Event& e) override;
private:
	Chroma::OrthographicCameraController m_CameraController;

	// TEMP
	Chroma::Ref<Chroma::VertexArray> m_SquareVA;
	Chroma::Ref<Chroma::Shader> m_Shader;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;

	Chroma::Ref<Chroma::Texture2D> m_Texture;

	Chroma::Ref<Chroma::Framebuffer> m_Framebuffer;
};