#pragma once
#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Rect.h"

namespace Chroma
{
	class Scene;

    class CameraComponent : public Component
    {
    public:
		
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(float x, float y, float width, float height);

		const Math::vec2& GetPosition() const { return position; }
		void SetPosition(const Math::vec2& pos) { position = pos; RecalculateViewMatrix(); }
		void SetPosition(float x, float y) { position = { x, y, }; RecalculateViewMatrix(); }

		const Math::uvec2& GetSize() const { return size; }
		void SetSize(const Math::uvec2& size) { this->size = size; RecalculateViewMatrix(); }
		void SetSize(uint32_t x, uint32_t y) { size = { x, y, }; RecalculateViewMatrix(); }


		float GetRotation() const { return rotation; }
		void SetRotation(float rotation) { rotation = rotation; RecalculateViewMatrix(); }

		const Math::mat4& GetProjectionMatrix() const { return projectionMatrix; }
		const Math::mat4& GetViewMatrix() const { return viewMatrix; }
		const Math::mat4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }

		static void SetPrimaryCamera(CameraComponent* camera);
		static CameraComponent& GetPrimaryCamera();
		void SetPrimary();
		bool IsPrimary();


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
		void RecalculateViewMatrix();

		Math::mat4 projectionMatrix;
		Math::mat4 viewMatrix;
		Math::mat4 viewProjectionMatrix;

		glm::vec2 position = { 0.0f, 0.0f };
		glm::uvec2 size = { 1920, 1080 };
		float rotation = 0.0f;
		static CameraComponent* primaryCamera;


		friend class Scene;
    };
}