#pragma once

#include "Collider.h"

namespace Chroma
{

	struct EdgeCollider : public Collider
	{
		CHROMA_COMPONENT(EdgeCollider, Collider);

		inline b2Shape *GetShapeHandle() override { return &m_Shape; }

		inline const ColliderType GetColliderType() const override { return ColliderType::Edge; }

		Math::vec2 GetPosition() const override;
		void SetPosition(const Math::vec2 &value) override;

		void SetOneSided(bool val);
		bool IsOneSided() const;

		void SetV0(const Math::vec2 &val);
		void SetV1(const Math::vec2 &val);
		void SetV2(const Math::vec2 &val);
		void SetV3(const Math::vec2 &val);

		Math::vec2 GetV0() const;
		Math::vec2 GetV1() const;
		Math::vec2 GetV2() const;
		Math::vec2 GetV3() const;

	private:
		b2EdgeShape m_Shape;
	};
}