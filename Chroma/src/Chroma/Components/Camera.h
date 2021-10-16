#pragma once
#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Rect.h"

namespace Chroma
{
	class Scene;
	class CameraSystem;

    class Camera : public Component
    {
    public:
		
		Camera() = default;
		Camera(const Camera&) = default;
		Camera(float width, float height);

		const Math::uvec2& GetSize() const { return size; }
		void SetSize(const Math::uvec2& size) { this->size = size; dirty = true; }
		void SetSize(uint32_t x, uint32_t y) { size = { x, y, }; dirty = true; }


		const Math::mat4& GetProjectionMatrix() const { return projectionMatrix; }
		const Math::mat4& GetViewMatrix() const { return viewMatrix; }
		const Math::mat4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }


		const std::string Name() const override
		{
			return StaticName();
		}

		const static std::string StaticName()
		{
			return "Camera";
		}

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;



	private:
		void RecalculateViewMatrix(const Math::vec2& position);

		Math::mat4 projectionMatrix;
		Math::mat4 viewMatrix;
		Math::mat4 viewProjectionMatrix;
		glm::uvec2 size = { 1920, 1080 };
		bool dirty = true;


		friend class Scene;
		friend class CameraSystem;
    };
}