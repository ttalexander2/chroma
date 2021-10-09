#pragma once
#include "Chroma/Math/Math.h"


namespace Polychrome
{
	class EditorCamera
	{
	private:
		static void RecalculateViewMatrix();
	public:
		static const Math::vec2& GetPosition() { return EditorCamera::position; }
		static void SetPosition(const Math::vec2& pos) { EditorCamera::position = pos; RecalculateViewMatrix(); }
		static void SetPosition(float x, float y) { EditorCamera::position = { x, y, }; RecalculateViewMatrix(); }

		static const Math::uvec2& GetSize() { return EditorCamera::size; }
		static void SetSize(const Math::uvec2& size) { EditorCamera::size = size; RecalculateViewMatrix(); }
		static void SetSize(uint32_t x, uint32_t y) { EditorCamera::size = { x, y, }; RecalculateViewMatrix(); }

		static const Math::mat4& GetProjectionMatrix() { return EditorCamera::projectionMatrix; }
		static const Math::mat4& GetViewMatrix() { return EditorCamera::viewMatrix; }
		static const Math::mat4& GetViewProjectionMatrix() { return EditorCamera::viewProjectionMatrix; }

		static const void SetZoom(float zoooom) { zoom = glm::max(0.05f, zoooom); RecalculateViewMatrix(); }
		static const float GetZoom() { return zoom; }


		static void ImGuiUpdate();
	private:
		static Math::mat4 projectionMatrix;
		static Math::mat4 viewMatrix;
		static Math::mat4 viewProjectionMatrix;

		static glm::vec2 position;
		static glm::uvec2 size;
		static float zoom;

	};
}