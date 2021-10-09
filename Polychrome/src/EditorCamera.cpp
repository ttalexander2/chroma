#include "EditorCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Viewport.h"

namespace Polychrome
{

	Math::mat4 EditorCamera::projectionMatrix;
	Math::mat4 EditorCamera::viewMatrix;
	Math::mat4 EditorCamera::viewProjectionMatrix;

	glm::vec2 EditorCamera::position{0,0};
	glm::uvec2 EditorCamera::size { 480, 270 };

	float EditorCamera::zoom = 1.f;

	void EditorCamera::RecalculateViewMatrix()
	{
		float aspectRatio = (float)size.x / (float)size.y;
		float left = -(float)(size.y / zoom) * aspectRatio * .5f;
		float right = (float)(size.y / zoom) * aspectRatio * .5f;
		float bottom = -(float)(size.y / zoom) * .5f;
		float top = (float)(size.y / zoom) * .5f;
		projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.f }) *
			glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

		viewMatrix = glm::inverse(transform);

		//OpenGL order, reverse for DirectX, as it uses row major order instead
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void EditorCamera::ImGuiUpdate()
	{
		static bool downInViewport = false;
		if (Viewport::IsViewportFocused())
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) && Viewport::IsViewportHovered())
			{
				downInViewport = true;
			}
			static Math::vec2 dragAmt{0,0};
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) && downInViewport)
			{
				auto rawDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
				Math::vec2 delta = { rawDelta.x - dragAmt.x, rawDelta.y - dragAmt.y };
				//delta = Viewport::ViewportPositionToWorld(delta);
				position = {position.x - delta.x / zoom, position.y + delta.y / zoom };
				RecalculateViewMatrix();
				ImGui::ResetMouseDragDelta();
				dragAmt = { rawDelta.x, rawDelta.y };
			}
			else
			{
				dragAmt = { 0,0 };
				downInViewport = false;
			}
		}
		
	}


}

