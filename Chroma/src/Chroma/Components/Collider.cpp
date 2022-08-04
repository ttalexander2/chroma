#include "chromapch.h"
#include "Collider.h"

#include "Chroma/Reflection/Reflection.h"


namespace Chroma
{
	void Collider::CreateReflectionModel()
	{
		Reflection::RegisterComponent<Collider, Component>();
		Reflection::RegisterComponentProperty<Collider, &Collider::Mask>("Mask");
		Reflection::RegisterComponentProperty<Collider, &Collider::Layer>("Layer");
		Reflection::RegisterComponentProperty<Collider, &Collider::SetFriction, &Collider::GetFriction>("Friction");
		Reflection::RegisterComponentProperty<Collider, &Collider::SetRestitution, &Collider::GetRestitution>("Restitution");
		Reflection::RegisterComponentProperty<Collider, &Collider::SetRestitutionThreshold, &Collider::GetRestitutionThreshold>("RestitutionThreshold");
		Reflection::RegisterComponentProperty<Collider, &Collider::SetDensity, &Collider::GetDensity>("Density");
		Reflection::RegisterComponentProperty<Collider, &Collider::SetSensor, &Collider::IsSensor>("Sensor");
	}



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
		else
		{
			return m_FixtureDef.friction;
		}
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
		else
		{
			return m_FixtureDef.restitution;
		}
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
		else
		{
			return m_FixtureDef.restitutionThreshold;
		}
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
		else
		{
			return m_FixtureDef.density;
		}
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
		else
		{
			return m_FixtureDef.isSensor;
		}
	}


} //namespace Chroma