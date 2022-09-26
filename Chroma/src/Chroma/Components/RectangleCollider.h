#pragma once

#include "Collider.h"

namespace Chroma
{
	struct RectangleCollider : public Collider
	{
		CHROMA_COMPONENT(RectangleCollider, Collider);

		void Initialize() override;
		b2Shape *GetShapeHandle() override { return &m_Shape; }

		const ColliderType GetColliderType() const override { return ColliderType::Edge; }

		Math::vec2 GetPosition() const override;
		void SetPosition(const Math::vec2 &value) override;

		void SetSize(float hx, float hy);
		void SetSize(const Math::vec2 &size);
		Math::vec2 GetSize() const;

		void SetRotation(float angle);
		float GetRotation() const;

	private:
		b2PolygonShape m_Shape;

		//Setting this value does not control rotation, it is only used to keep track of the rotation applied to the polygon
		float m_Rotation = 0.f;
	};
}
