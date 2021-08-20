#pragma once

#include <Chroma.h>
#include "EditorConfig.h"
#include <Chroma/Renderer/Framebuffer.h>
#include <Chroma/Systems/SpriteRendererSystem.h>


namespace Polychrome
{
	class EditorApp : public Chroma::Application
	{
	public:
		EditorApp();
		~EditorApp();

		void Init() override;
		void OnEvent(Chroma::Event& e) override;
		void Update(Chroma::Time time) override;
		void Draw(Chroma::Time time) override;
		void ImGuiDraw(Chroma::Time time) override;

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
		void SaveScene();

		Chroma::OrthographicCameraController& GetCameraController() { return m_CameraController; }

		EditorConfig Config;
		static Chroma::Scene* CurrentScene;
		static bool SceneRunning;
		static bool ScenePaused;
		static bool PreviewSprites;

		static ImFont* LargeIcons;

	private:
		Chroma::OrthographicCameraController m_CameraController;

		bool m_ViewportFocused = false;

		std::string CurrentScenePath;

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

		Chroma::SpriteRendererSystem sprite_system;
	};
}


