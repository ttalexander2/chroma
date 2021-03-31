#include "Viewport.h"
#include <imgui.h>


namespace Polychrome
{
	static glm::vec2 s_ViewportSize = glm::vec2(1270, 720);

	static bool shouldUpdate = true;
	static Chroma::Window* s_ViewportWindow;


	void Viewport::Render(Chroma::Ref<Chroma::Framebuffer> frame_buffer)
	{
		ImGui::Begin("Viewport");

		bool viewport_focused = ImGui::IsWindowFocused();
		Chroma::Application::Get().GetImGuiLayer()->BlockEvents(!viewport_focused);


		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		float width, height;
		// Case 1: Window is wider than 16:9
		if (viewportPanelSize.x / viewportPanelSize.y > 16.0f / 9.0f)
		{
			height = viewportPanelSize.y;
			width = viewportPanelSize.y * (16.0f / 9.0f);
		}
		else
		{
			height = viewportPanelSize.x * (9.0f / 16.0f);
			width = viewportPanelSize.x;
		}
		height = glm::round(height);
		width = glm::round(width);

		if ((s_ViewportSize.x != width || s_ViewportSize.y != height) && width > 0 && height > 0)
		{
			frame_buffer->Resize((uint32_t)width, (uint32_t)height);
			s_ViewportSize = { width, height };
		}

		uint32_t textureID = frame_buffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ s_ViewportSize.x, s_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}
}