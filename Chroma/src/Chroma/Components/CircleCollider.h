#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec2.h"

namespace Chroma
{
	/// @brief 2D circle collider. Collision is calulated based on radius, offset, and transform position.
	struct CircleCollider : public Component
	{
		CHROMA_COMPONENT(CircleCollider, Component);


		/// @brief Radius of the circle collider.
		float Radius = 1.0f;
		/// @brief 2D offset of the circle collider.
		Math::vec2 Offset { 0.0f, 0.0f };


		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;
	};
}

