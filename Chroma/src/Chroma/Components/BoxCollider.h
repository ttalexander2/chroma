#pragma once

#include "Chroma/Components/Collider.h"
#include "Chroma/Math/vec2.h"

#include "Chroma/Systems/CollisionSystem.h"

namespace Chroma
{

	/// @brief 2D Box collider. Bounds are calculated based on Transorm position and offset.
	struct BoxCollider : public Collider
	{
		CHROMA_COMPONENT(BoxCollider, Collider);

		/// @brief 2D Min of the Box Collider.
		Math::vec2 Min{ 0.0f, 0.0f };

		Math::vec2 Max{ 20.0f, 20.0f };

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

		bool IsColliding() const { return m_Colliding; }

	private:
		bool m_Colliding = false;
		int m_Proxy = 0;

		friend class CollisionSystem;

	};
}

