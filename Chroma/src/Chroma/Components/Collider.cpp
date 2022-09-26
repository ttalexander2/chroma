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

	Reflection::TypeFactory<Collider> Collider::RegisterType()
	{
		Reflection::Register<Vertex>("Collider::Vertex")
				.Data<&Vertex::vertex>("vertex")
				.Data<&Vertex::normal>("normal");

		Reflection::Register<ColliderType>("Collider::ColliderType")
				.Data<ColliderType::Circle>("Circle")
				.Data<ColliderType::Edge>("Edge")
				.Data<ColliderType::Polygon>("Polygon")
				.Data<ColliderType::Rectangle>("Rectangle");

		return Reflection::Register<Collider>("Collider")
		       .Base<Component>()
		       .Data<&Collider::Mask>("Mask")
		       .Data<&Collider::Layer>("Layer")
		       .Data<&Collider::SetPosition, &Collider::GetPosition>("Position", false)
		       .Data<&Collider::SetFriction, &Collider::GetFriction>("Friction")
		       .Data<&Collider::SetRestitution, &Collider::GetRestitution>("Restitution")
		       .Data<&Collider::SetRestitutionThreshold, &Collider::GetRestitutionThreshold>("RestitutionThreshold")
		       .Data<&Collider::SetDensity, &Collider::GetDensity>("Density")
		       .Data<&Collider::SetSensor, &Collider::IsSensor>("Sensor")
		       .Data<nullptr, &Collider::GetColliderType>("ColliderType", false);
	}
} //namespace Chroma
