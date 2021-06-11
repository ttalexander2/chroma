#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Mat4x4.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Chroma/ImGui/ImGuiComponents/Vec3WithLabels.h"

#include "imgui.h"


namespace Chroma
{
	struct Transform : public Component
	{
		Math::vec3 Position = { 0.0f, 0.0f, 0.0f };
		Math::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		Math::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const Math::mat4& transform)
		{
		}

		Math::mat4 GetTransform() const;

		const std::string Name() const override
		{
			return "Transform";
		}

		const bool AllowMultiple() const override { return false; };

		void DrawImGui() override;

		void Serialize(YAML::Emitter& out) override;


		void Deserialize(YAML::Node& node) override;

	private:
		const bool IsTransform() const override
		{
			return true;
		}
		

	};
}


