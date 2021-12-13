#include "EditorCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Viewport.h"
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"
#include <imgui.h>

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
		static bool cursorSet = false;



		if (Viewport::IsViewportFocused())
		{
			static bool holding = false;
			if (ImGui::IsKeyDown((int)Chroma::Input::Key::SPACE))
			{
				glfwSetCursor((GLFWwindow*)Chroma::Application::Get().GetWindow().GetNativeWindow(), glfwCreateStandardCursor(GLFW_HAND_CURSOR));
				holding = true;
			}
			else
			{
				holding = false;
			}

			if ((ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || (ImGui::IsKeyDown((int)Chroma::Input::Key::SPACE) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))) && Viewport::IsViewportHovered())
			{

					glfwSetCursor((GLFWwindow*)Chroma::Application::Get().GetWindow().GetNativeWindow(), glfwCreateStandardCursor(GLFW_HAND_CURSOR));
					cursorSet = true;
				
					
				downInViewport = true;
			}
			static Math::vec2 dragAmt{0,0};
			if ((ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || (ImGui::IsKeyDown((int)Chroma::Input::Key::SPACE) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))) && downInViewport)
			{
				auto mouse = ImGuiMouseButton_Left;
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
					mouse = ImGuiMouseButton_Middle;
				auto rawDelta = ImGui::GetMouseDragDelta(mouse);
				Math::vec2 delta = { rawDelta.x - dragAmt.x, rawDelta.y - dragAmt.y };
				//delta = Viewport::ViewportPositionToWorld(delta);
				position = {position.x - delta.x / zoom, position.y + delta.y / zoom };
				RecalculateViewMatrix();
				//ImGui::ResetMouseDragDelta();
				dragAmt = { rawDelta.x, rawDelta.y };
			}
			else
			{
				if (!holding)
					glfwSetCursor((GLFWwindow*)Chroma::Application::Get().GetWindow().GetNativeWindow(), glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
				cursorSet = false;
				
					
				dragAmt = { 0,0 };
				downInViewport = false;
			}
		}
		
	}


}

