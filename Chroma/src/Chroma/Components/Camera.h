#pragma once
#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Rect.h"

namespace Chroma
{
	class Scene;
	class CameraSystem;

    struct Camera : public Component
    {
		CHROMA_COMPONENT(Camera, Component);

    	
		const Math::vec2& GetPosition() const { return position; }
		void SetPosition(const Math::vec2& pos) { if (position != pos) dirty = true; position = pos; }

		const Math::uvec2& GetSize() const { return size; }
		void SetSize(const Math::uvec2& size) { this->size = size; dirty = true; }


		const Math::mat4& GetProjectionMatrix() const { return projectionMatrix; }
		const Math::mat4& GetViewMatrix() const { return viewMatrix; }
		const Math::mat4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

		Math::mat4 projectionMatrix{};
		Math::mat4 viewMatrix{};
		Math::mat4 viewProjectionMatrix{};
		glm::uvec2 size = { 1920, 1080 };
		Math::vec2 position = { 0,0 };
		bool dirty = true;


		friend class Scene;
		friend class CameraSystem;
    };
}