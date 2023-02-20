#pragma once

#include "Collider.h"

namespace Chroma
{
	struct CircleCollider : Collider
	{
		CHROMA_COMPONENT(CircleCollider, Collider);

		b2Shape *GetShapeHandle() override { return &m_Shape; }

		const ColliderType GetColliderType() const override { return ColliderType::Circle; }

		float GetRadius() const;
		void SetRadius(float value);

		Math::vec2 GetPosition() const override;
		void SetPosition(const Math::vec2 &value) override;

	private:
		b2CircleShape m_Shape;
	};
}
