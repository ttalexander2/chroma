#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec2.h"

namespace Chroma
{
	/// @brief 2D circle collider. Collision is calulated based on radius, offset, and transform position.
	struct CircleCollider2D : Component
	{
		/// @brief Constructs an empty CircleCollider2D
		CircleCollider2D() = default;
		/// @brief Constructs a new CircleCollider2D from an existing CircleCollider2D.
		/// @param  AudioSource to copy.
		CircleCollider2D(const CircleCollider2D&) = default;

		/// @brief Radius of the circle collider.
		float Radius = 1.0f;
		/// @brief 2D offset of the circle collider.
		Math::vec2 Offset { 0.0f, 0.0f };

		const std::string Name() const override
		{
			return "Circle Collider 2D";
		}

		void DrawImGui() override;

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;
	};
}

