#include "chromapch.h"
#include "CircleCollider.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	float CircleCollider::GetRadius() const
	{
		return m_Shape.m_radius;
	}

	void CircleCollider::SetRadius(float value)
	{
		m_Shape.m_radius = value;
	}

	Math::vec2 CircleCollider::GetPosition() const
	{
		return Math::vec2(m_Shape.m_p.x / Physics::GetScale(), m_Shape.m_p.y / Physics::GetScale());
	}

	void CircleCollider::SetPosition(const Math::vec2 &value)
	{
		m_Shape.m_p = { value.x * Physics::GetScale(), value.y * Physics::GetScale() };
	}

	Reflection::TypeFactory<CircleCollider> CircleCollider::RegisterType()
	{
		return Reflection::Register<CircleCollider>("CircleCollider")
				.Base<Collider>()
				.Data<&CircleCollider::SetRadius, &CircleCollider::GetRadius>("Radius");
	}

}