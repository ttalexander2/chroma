#pragma once

#include "Collider.h"

namespace Chroma
{
	
	struct CircleCollider : public Collider
	{
		CHROMA_COMPONENT(CircleCollider, Collider);

		inline b2Shape *GetShapeHandle() { return &m_Shape; }

		inline const ColliderType GetColliderType() const override { return ColliderType::Circle; }

		float GetRadius() const;
		void SetRadius(float value);

		Math::vec2 GetPosition() const override;
		void SetPosition(const Math::vec2 &value) override;

	private:
		b2CircleShape m_Shape;
	};
}