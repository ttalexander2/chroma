#include "chromapch.h"
#include "EdgeCollider.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	void EdgeCollider::CreateReflectionModel()
	{
		Reflection::RegisterComponent<EdgeCollider, Collider>();
		Reflection::RegisterComponentProperty<EdgeCollider, &EdgeCollider::SetOneSided, &EdgeCollider::IsOneSided>("OneSided");
		Reflection::RegisterComponentProperty<EdgeCollider, &EdgeCollider::SetV0, &EdgeCollider::GetV0>("V0");
		Reflection::RegisterComponentProperty<EdgeCollider, &EdgeCollider::SetV1, &EdgeCollider::GetV1>("V1");
		Reflection::RegisterComponentProperty<EdgeCollider, &EdgeCollider::SetV2, &EdgeCollider::GetV2>("V2");
		Reflection::RegisterComponentProperty<EdgeCollider, &EdgeCollider::SetV3, &EdgeCollider::GetV3>("V3");
	}

	Math::vec2 EdgeCollider::GetPosition() const
	{
		return Math::vec2(m_Shape.m_vertex2.x / Physics::GetScale(), m_Shape.m_vertex2.y / Physics::GetScale());
	}

	void EdgeCollider::SetPosition(const Math::vec2 &value)
	{
		Math::vec2 val = value * Physics::GetScale();
		b2Vec2 currentOrigin = m_Shape.m_vertex2;
		m_Shape.m_vertex0 = { m_Shape.m_vertex0.x - currentOrigin.x + val.x, m_Shape.m_vertex0.y - currentOrigin.y + val.y };
		m_Shape.m_vertex1 = { m_Shape.m_vertex1.x - currentOrigin.x + val.x, m_Shape.m_vertex1.y - currentOrigin.y + val.y };
		m_Shape.m_vertex2 = { m_Shape.m_vertex2.x - currentOrigin.x + val.x, m_Shape.m_vertex2.y - currentOrigin.y + val.y };
		m_Shape.m_vertex3 = { m_Shape.m_vertex3.x - currentOrigin.x + val.x, m_Shape.m_vertex3.y - currentOrigin.y + val.y };
	}

	void EdgeCollider::SetOneSided(bool val)
	{
		m_Shape.m_oneSided = val;
	}

	bool EdgeCollider::IsOneSided() const
	{
		return m_Shape.m_oneSided;
	}

	void EdgeCollider::SetV0(const Math::vec2 &val)
	{
		m_Shape.m_vertex0 = { val.x * Physics::GetScale(), val.y * Physics::GetScale() };
	}

	void EdgeCollider::SetV1(const Math::vec2 &val)
	{
		m_Shape.m_vertex1 = { val.x * Physics::GetScale(), val.y * Physics::GetScale() };
	}

	void EdgeCollider::SetV2(const Math::vec2 &val)
	{
		m_Shape.m_vertex2 = { val.x * Physics::GetScale(), val.y * Physics::GetScale() };
	}

	void EdgeCollider::SetV3(const Math::vec2 &val)
	{
		m_Shape.m_vertex3 = { val.x * Physics::GetScale(), val.y * Physics::GetScale() };
	}

	Math::vec2 EdgeCollider::GetV0() const
	{
		return Math::vec2(m_Shape.m_vertex0.x / Physics::GetScale(), m_Shape.m_vertex0.y / Physics::GetScale());
	}

	Math::vec2 EdgeCollider::GetV1() const
	{
		return Math::vec2(m_Shape.m_vertex1.x / Physics::GetScale(), m_Shape.m_vertex1.y / Physics::GetScale());
	}

	Math::vec2 EdgeCollider::GetV2() const
	{
		return Math::vec2(m_Shape.m_vertex2.x / Physics::GetScale(), m_Shape.m_vertex2.y / Physics::GetScale());
	}

	Math::vec2 EdgeCollider::GetV3() const
	{
		return Math::vec2(m_Shape.m_vertex3.x / Physics::GetScale(), m_Shape.m_vertex3.y / Physics::GetScale());
	}
}