#pragma once

#define CHROMA_DEBUG_LOG


#include <Chroma.h>
#include "EditorConfig.h"
#include <Chroma/Renderer/Framebuffer.h>
#include <Chroma/Systems/SpriteRendererSystem.h>
#include "EditorCamera.h"


#define CHROMA_EDITOR


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

		bool OnMouseScrolled(Chroma::MouseScrolledEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
		void SaveScene();

		static EditorCamera Camera;

		EditorConfig Config;
		static Chroma::Scene* CurrentScene;
		static std::string CurrentScenePath;
		static bool SceneRunning;
		static bool ScenePaused;
		static bool PreviewSprites;

		static ImFont* LargeIcons;
		static ImFont* LargeFont;

	private:

		bool m_ViewportFocused = false;



		struct ProfileResult
		{
			const char* Name;
			float Time;
		};

		std::vector<ProfileResult> m_ProfileResults;

		Chroma::Ref<Chroma::Framebuffer> m_Framebuffer;

		Chroma::SpriteRendererSystem sprite_system;
	};
}


