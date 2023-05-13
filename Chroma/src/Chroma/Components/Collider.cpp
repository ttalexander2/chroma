#include "chromapch.h"
#include "Collider.h"

#include "Chroma/Reflection/Reflection.h"


namespace Chroma
{
	void Collider::Initialize()
	{
		SetDensity(1.f);
	}

	void Collider::SetFriction(float value)
	{
		if (m_Fixture)
		{
			m_Fixture->SetFriction(value);
		}
		else
		{
			m_FixtureDef.friction = value;
		}
	}

	float Collider::GetFriction() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetFriction();
		}
		return m_FixtureDef.friction;
	}

	void Collider::SetRestitution(float value)
	{
		if (m_Fixture)
		{
			m_Fixture->SetRestitution(value);
		}
		else
		{
			m_FixtureDef.restitution = value;
		}
	}

	float Collider::GetRestitution() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetRestitution();
		}
		return m_FixtureDef.restitution;
	}

	void Collider::SetRestitutionThreshold(float value)
	{
		if (m_Fixture)
		{
			m_Fixture->SetRestitutionThreshold(value);
		}
		else
		{
			m_FixtureDef.restitutionThreshold = value;
		}
	}

	float Collider::GetRestitutionThreshold() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetRestitutionThreshold();
		}
		return m_FixtureDef.restitutionThreshold;
	}

	void Collider::SetDensity(float density)
	{
		if (m_Fixture)
		{
			m_Fixture->SetDensity(density);
		}
		else
		{
			m_FixtureDef.density = density;
		}
	}

	float Collider::GetDensity() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetDensity();
		}
		return m_FixtureDef.density;
	}

	void Collider::SetSensor(bool sensor)
	{
		if (m_Fixture)
		{
			m_Fixture->SetSensor(sensor);
		}
		else
		{
			m_FixtureDef.isSensor = sensor;
		}
	}

	bool Collider::IsSensor() const
	{
		if (m_Fixture)
		{
			return m_Fixture->IsSensor();
		}
		return m_FixtureDef.isSensor;
	}

	Reflection::type_factory<Collider> Collider::register_type()
	{
		Reflection::register_type<Vertex>("Collider::Vertex")
				.data<&Vertex::vertex>("vertex")
				.data<&Vertex::normal>("normal");

		Reflection::register_type<ColliderType>("Collider::ColliderType")
				.data<ColliderType::Circle>("Circle")
				.data<ColliderType::Edge>("Edge")
				.data<ColliderType::Polygon>("Polygon")
				.data<ColliderType::Rectangle>("Rectangle");

		return Reflection::register_type<Collider>("Collider")
		       .base<Component>() 
		       .data<&Collider::Mask>("Mask")
		       .data<&Collider::Layer>("Layer")
		       .data<&Collider::GetPosition, &Collider::SetPosition>("Position")
		       .data<&Collider::GetFriction, &Collider::SetFriction>("Friction")
		       .data<&Collider::GetRestitution, &Collider::SetRestitution>("Restitution")
		       .data<&Collider::GetRestitutionThreshold, &Collider::SetRestitutionThreshold>("RestitutionThreshold")
		       .data<&Collider::GetDensity, &Collider::SetDensity>("Density")
		       .data<&Collider::IsSensor, &Collider::SetSensor>("Sensor")
		       .data<&Collider::GetColliderType, nullptr>("ColliderType");
	}
} //namespace Chroma
