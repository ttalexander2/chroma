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

		Math::vec2 Position = { 0, 0 };
		float Rotation = 0.0f;
		Math::vec2 Scale = { 1.0f, 1.0f };

		std::vector<EntityID> Children;
		EntityID Parent{ ENTITY_NULL };

		const bool HasChildren() const { return Children.size() > 0; };
		const bool IsChild() const { return Parent != ENTITY_NULL; };

		const bool HasChild(EntityID child) const { return std::find(Children.begin(), Children.end(), child) != Children.end(); }


		/// @brief Get the transform as a 4x4 matrix.
		/// @return mat4 transform.
		Math::mat4 GetTransform() const;

		void Serialize(YAML::Emitter &out) override;

		void Deserialize(YAML::Node& node) override;

		const inline void SetEnabled(bool enabled) override { m_Enabled = true; }
		

	};
}


