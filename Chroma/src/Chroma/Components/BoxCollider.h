#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/vec2.h"

#include "Chroma/Systems/CollisionSystem.h"

namespace Chroma
{

	/// @brief 2D Box collider. Bounds are calculated based on Transorm position and offset.
	struct BoxCollider : Component
	{
		/// @brief 2D Min of the Box Collider.
		Math::vec2 Min{ 0.0f, 0.0f };

		Math::vec2 Max{ 20.0f, 20.0f };

		/// @brief Constructs an empty Box Collider.
		BoxCollider() = default;
		/// @brief Constructs a Box Collider from an existing Box Collider.
		/// @param  BoxCollider to copy.
		BoxCollider(const BoxCollider&) = default;
		/// @brief Constructs a new Box Collider.
		/// @param bounds 2D bounds of the box collider.
		/// @param offset 2D offset of the box collider.
		BoxCollider(const Math::vec2& min, const Math::vec2& max)
			: Min(min), Max(max)
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

		bool IsColliding() const { return m_Colliding; }

	private:
		bool m_Colliding = false;
		int m_Proxy = 0;
		EntityID m_ID;

		friend class CollisionSystem;

	};
}

