#include "Viewport.h"
#include <imgui.h>
#include "Fonts/IconsForkAwesome.h"
#include <ImGuizmo.h>
#include "Hierarchy.h"
#include "EditorApp.h"
#include <Chroma/Components/Transform.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <Chroma/Math/Decompose.h>

namespace Polychrome
{
	static glm::vec2 s_ViewportSize = glm::vec2(1270, 720);

	static bool shouldUpdate = true;
	static Chroma::Window* s_ViewportWindow;

	bool Viewport::Open = true;

	static const float identityMatrix[16] =
	{ 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };


	void Viewport::Draw(Chroma::Ref<Chroma::Framebuffer> frame_buffer)
	{
		if (Open)
		{
			ImGui::Begin(ICON_FK_GAMEPAD " Viewport", &Open);

			bool viewport_focused = ImGui::IsWindowFocused();
			Chroma::Application::Get().GetImGuiLayer()->BlockEvents(!viewport_focused && !ImGui::IsWindowHovered());


			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			float width, height;
			// Case 1: Window is wider than 16:9
			if (viewportPanelSize.x / viewportPanelSize.y > 16.0f / 9.0f)
			{
				height = viewportPanelSize.y;
				width = viewportPanelSize.y * (16.0f / 9.0f);
			}
			// Case 1: Window is narrower than 16:9
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
			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ s_ViewportSize.x, s_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			//ImGuizmo

			

			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
			{
				ImGuizmo::SetOrthographic(true);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());
				auto& cam = dynamic_cast<EditorApp&>(EditorApp::Get()).GetCameraController();
				const Math::mat4& camView = cam.GetCamera().GetViewMatrix();
				const Math::mat4& camProj = cam.GetCamera().GetProjectionMatrix();

			

				auto transform = EditorApp::CurrentScene->GetComponent<Chroma::Transform>(Hierarchy::SelectedEntity);
				glm::mat4 transformMat = transform->GetTransform();

				const float snap = 1.0f;
				//ImGuizmo::DrawGrid(glm::value_ptr(camView), glm::value_ptr(camProj), identityMatrix, 32.f);
				ImGuizmo::Manipulate(glm::value_ptr(camView), glm::value_ptr(camProj),
					ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transformMat));

				if (ImGuizmo::IsUsing())
				{

					glm::vec3 translation = transform->Position;
					glm::vec3 rotation = transform->Rotation;
					glm::vec3 scale = transform->Scale;

					if (Chroma::DecomposeTransform(transformMat, translation, rotation, scale))
					{
						//rotation.x = glm::degrees(rotation.x);
						//rotation.y = glm::degrees(rotation.y);
						//rotation.z = glm::degrees(rotation.z);


						glm::vec3 deltaRotation = rotation - transform->Rotation;

						//CHROMA_CORE_WARN("Position: [{},{},{}]", translation.x, translation.y, translation.z);
						//CHROMA_CORE_WARN("Rotation: [{},{},{}]", rotation.x, rotation.y, rotation.z);
						//CHROMA_CORE_WARN("Scale: [{},{},{}]", scale.x, scale.y, scale.z);

						transform->Position = translation;
						transform->Rotation += deltaRotation;
						transform->Scale = scale;
					}

					
				}
			}

			ImGui::End();
		}
		
	}
}