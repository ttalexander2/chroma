#pragma once

#define CHROMA_DEBUG_LOG


#include <Chroma.h>
#include "EditorConfig.h"
#include <Chroma/Renderer/Framebuffer.h>
#include <Chroma/Renderer/UniformBuffer.h>
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

		static void DrawSelectionMask(Chroma::Time time);

		static EditorCamera Camera;

		EditorConfig Config;
		static Chroma::Scene* CurrentScene;
		static std::string CurrentScenePath;
		static bool SceneRunning;
		static bool ScenePaused;
		static bool PreviewSprites;

		static ImFont* LargeIcons;
		static ImFont* LargeFont;

		enum class MessageSeverity
		{
			Error = 0,
			Warning = 1,
			Info = 2
		};

		static void SetInfoMessage(const std::string& message, MessageSeverity severity)
		{
			InfoMessage = message;
			InfoSeverity = severity;
		}

	private:

		bool m_ViewportFocused = false;

		static std::string InfoMessage;
		static MessageSeverity InfoSeverity;

		struct ProfileResult
		{
			const char* Name;
			float Time;
		};

		std::vector<ProfileResult> m_ProfileResults;

		Chroma::Ref<Chroma::Framebuffer> m_Framebuffer;
		Chroma::Ref<Chroma::Framebuffer> m_GuizmoFramebuffer;
		Chroma::Ref<Chroma::Shader> m_HighlightShader;

		Chroma::SpriteRendererSystem sprite_system;
	};
}


