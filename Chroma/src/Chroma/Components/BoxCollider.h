#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/vec2.h"

namespace Chroma
{

	/// @brief 2D Box collider. Bounds are calculated based on Transorm position and offset.
	struct BoxCollider : Component
	{
		/// @brief 2D Bounds of the Box Collider.
		Math::vec2 Bounds{ 1.0f, 1.0f };
		/// @brief 2D Offset of the Box Collider.
		Math::vec2 Offset{ 0.0f, 0.0f };

		/// @brief Constructs an empty Box Collider.
		BoxCollider() = default;
		/// @brief Constructs a Box Collider from an existing Box Collider.
		/// @param  BoxCollider to copy.
		BoxCollider(const BoxCollider&) = default;
		/// @brief Constructs a new Box Collider.
		/// @param bounds 2D bounds of the box collider.
		/// @param offset 2D offset of the box collider.
		BoxCollider(const Math::vec2& bounds, const Math::vec2& offset)
			: Bounds(bounds), Offset(offset)
		{
		}

		const std::string Name() const override
		{
			return StaticName();
		}

		const static std::string StaticName()
		{
			return "Box Collider";
		}

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	};
}

