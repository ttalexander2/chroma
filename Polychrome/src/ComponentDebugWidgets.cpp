#include "ComponentDebugWidgets.h"
#include "EditorApp.h"
#include "Hierarchy.h"
#include "Fonts/IconsForkAwesome.h"
#include "imgui_internal.h"
#include "Viewport.h"

namespace Polychrome
{
	void ComponentDebugWidgets::DrawWidgets()
	{
		//CAMERA
		Chroma::Renderer2D::DrawRect(Chroma::CameraComponent::GetPrimaryCamera().GetPosition(), Chroma::CameraComponent::GetPrimaryCamera().GetSize(), 1.f, { 0.2f, 0.3f , 0.8f , .8f });


		//COMPONENTS
		if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
		{
			for (Chroma::Component* comp : EditorApp::CurrentScene->GetAllComponents(Hierarchy::SelectedEntity))
			{

			}
		}
	}

	void ComponentDebugWidgets::DrawIcons()
	{
		float y = ImGui::GetCursorPosY();
		//CAMERA
		auto& camPos = Chroma::CameraComponent::GetPrimaryCamera().GetPosition();
		auto& camSize = Chroma::CameraComponent::GetPrimaryCamera().GetSize();
		Math::vec2 pos = { camPos.x - 10.f / EditorApp::Camera.GetZoom() , camPos.y + 10.f / EditorApp::Camera.GetZoom() + camSize.y / 2.f };
		Math::vec2 screen = Viewport::WorldToViewportPosition(pos);
		if (y - screen.y < ImGui::GetWindowContentRegionMax().y - 15.f)
		{
			ImGui::SetCursorPos({ screen.x, y - screen.y });
			ImGui::Text(ICON_FK_CAMERA);
		}


		//COMPONENTS
	}
}