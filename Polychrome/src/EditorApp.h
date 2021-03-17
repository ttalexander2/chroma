#pragma once

#include <Chroma.h>


namespace Polychrome
{
	class EditorApp : public Chroma::Application
	{
	public:
		EditorApp();
		~EditorApp() = default;

		void Initialize() override;
		void OnEvent(Chroma::Event& e) override;
		void Update(Chroma::Time time) override;
		void Draw(Chroma::Time time) override;
		void ImGuiDraw(Chroma::Time time) override;

	private:
		Chroma::OrthographicCameraController m_CameraController;

		bool m_ViewportFocused = false;

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
}


