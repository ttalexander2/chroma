#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Mat4x4.h"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"


namespace Chroma
{
	/// @brief Transform component, provides position, rotation and scale for entities.
	struct Transform : public Component
	{
		/// @brief Position
		Math::vec2 Position = { 0.0f, 0.0f };
		/// @brief Rotation
		float Rotation = 0.0f;
		/// @brief Scale
		Math::vec2 Scale = { 1.0f, 1.0f };

		/// @brief Constructs an empty Transform.
		Transform() = default;
		/// @brief Constructs a Transform component from an existing Transform.
		/// @param  Transform to copy.
		Transform(const Transform&) = default;
		/// @brief Constructs a transorm from a 4x4 Matrix.
		/// @param transform 
		Transform(const Math::mat4& transform)
		{
		}


		/// @brief Get the transform as a 4x4 matrix.
		/// @return mat4 transform.
		Math::mat4 GetTransform() const;

		const std::string Name() const override
		{
			return StaticName();
		}

		const static std::string StaticName()
		{
			return "Transform";
		}

		const bool AllowMultiple() const override { return false; };

		void Serialize(YAML::Emitter& out) override;


		void Deserialize(YAML::Node& node) override;

	private:
		const bool IsTransform() const override
		{
			return true;
		}
		

	};
}


