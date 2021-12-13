#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Mat4x4.h"
#include "glm/gtc/matrix_transform.hpp"


namespace Chroma
{
	/// @brief Transform component, provides position, rotation and scale for entities.
	struct Transform : public Component
	{
		CHROMA_COMPONENT(Transform, Component);

		/// @brief Position
		Math::vec2 Position = { 0.0f, 0.0f };
		/// @brief Rotation
		float Rotation = 0.0f;
		/// @brief Scale
		Math::vec2 Scale = { 1.0f, 1.0f };


		/// @brief Get the transform as a 4x4 matrix.
		/// @return mat4 transform.
		Math::mat4 GetTransform() const;

		void Serialize(YAML::Emitter& out) override;


		void Deserialize(YAML::Node& node) override;

		const inline void SetEnabled(bool enabled) override { m_Enabled = true; }
		

	};
}


