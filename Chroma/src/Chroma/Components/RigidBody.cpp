#include "chromapch.h"
#include "RigidBody.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{

	void RigidBody::CreateReflectionModel()
	{
		Reflection::RegisterComponent<RigidBody, Component>();
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetBodyType, &RigidBody::GetBodyType>("BodyType");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetLinearVelocity, &RigidBody::GetLinearVelocity>("LinearVelocity"); 
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetAngularVelocity, &RigidBody::GetAngularVelocity>("AngularVelocity");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetLinearDamping, &RigidBody::GetLinearDamping>("LinearDamping");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetAngularDamping, &RigidBody::GetAngularDamping>("AngularDamping");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetSleepingAllowed, &RigidBody::IsSleepingAllowed>("AllowSleep");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetAwake, &RigidBody::IsAwake>("StartAwake");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetFixedRotation, &RigidBody::IsFixedRotation>("FixedRotation");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetBullet, &RigidBody::IsBullet>("Bullet");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::SetGravityScale, &RigidBody::GetGravityScale>("GravityScale");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::m_Mass>("Mass");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::m_CenterOfMass>("Center");
		Reflection::RegisterComponentProperty<RigidBody, &RigidBody::m_Inertia>("Inertia");

	}

	void RigidBody::Initialize()
	{
		m_BodyDefinition.userData.pointer = reinterpret_cast<uintptr_t>(&m_EntityID);
		m_BodyDefinition.type = b2_dynamicBody;
		m_BodyDefinition.allowSleep = true;
		m_BodyDefinition.angle = 0;
		m_BodyDefinition.bullet = false;
		m_BodyDefinition.enabled = true;
		m_BodyDefinition.fixedRotation = false;
		m_BodyDefinition.gravityScale = 1.f;
		m_BodyDefinition.linearDamping = 0.f;
		m_BodyDefinition.linearVelocity = { 0.f, 0.f };
	}

	const void RigidBody::SetBodyType(BodyType type)
	{
		switch (type)
		{
			case BodyType::DYNAMIC:
				m_BodyDefinition.type = b2_dynamicBody;
				break;
			case BodyType::KINEMATIC:
				m_BodyDefinition.type = b2_kinematicBody;
				break;
			case BodyType::STATIC:
				m_BodyDefinition.type = b2_staticBody;
				break;
			default:
				m_BodyDefinition.type = b2_staticBody;
				break;
		}

	}

	RigidBody::BodyType RigidBody::GetBodyType() const
	{
		switch (m_BodyDefinition.type)
		{
			case b2_dynamicBody:
				return BodyType::DYNAMIC;
			case b2_kinematicBody:
				return BodyType::KINEMATIC;
			case b2_staticBody:
				return BodyType::STATIC;
		}
		return BodyType::STATIC;
	}

	void RigidBody::SetLinearVelocity(const Math::vec2 &velocity)
	{
		if (m_Body)
		{
			m_Body->SetLinearVelocity({ velocity.x, velocity.y });
		}
		else
		{
			m_BodyDefinition.linearVelocity = { velocity.x, velocity.y };
		}
	}

	Math::vec2 RigidBody::GetLinearVelocity() const
	{
		if (m_Body)
		{
			b2Vec2 v = m_Body->GetLinearVelocity();
			return { v.x, v.y };
		}
		else
		{
			return { m_BodyDefinition.linearVelocity.x, m_BodyDefinition.linearVelocity.y };
		}
	}

	void RigidBody::SetAngularVelocity(float velocity)
	{
		if (m_Body)
		{
			m_Body->SetAngularVelocity(velocity);
		}
		else
		{
			m_BodyDefinition.angularVelocity = velocity;
		}
	}

	float RigidBody::GetAngularVelocity() const
	{
		if (m_Body)
		{
			return m_Body->GetAngularVelocity();
		}
		else
		{
			return m_BodyDefinition.angularVelocity;
		}

	}

	void RigidBody::SetLinearDamping(float damping)
	{
		if (m_Body)
		{
			m_Body->SetLinearDamping(damping);
		}
		else
		{
			m_BodyDefinition.linearDamping = damping;
		}
	}

	float RigidBody::GetLinearDamping() const
	{
		if (m_Body)
		{
			return m_Body->GetLinearDamping();
		}
		else
		{
			return m_BodyDefinition.linearDamping;
		}
	}

	void RigidBody::SetAngularDamping(float damping)
	{
		if (m_Body)
		{
			m_Body->SetAngularDamping(damping);
		}
		else
		{
			m_BodyDefinition.angularDamping = damping;
		}
	}

	float RigidBody::GetAngularDamping() const
	{
		if (m_Body)
		{
			return m_Body->GetAngularDamping();
		}
		else
		{
			return m_BodyDefinition.angularDamping;
		}
	}

	void RigidBody::SetSleepingAllowed(bool value)
	{
		if (m_Body)
		{
			m_Body->SetSleepingAllowed(value);
		}
		else
		{
			m_BodyDefinition.allowSleep = value;
		}
	}

	bool RigidBody::IsSleepingAllowed() const
	{
		if (m_Body)
		{
			return m_Body->IsSleepingAllowed();
		} 
		else
		{
			return m_BodyDefinition.allowSleep;
		}
	}

	void RigidBody::SetAwake(bool value)
	{
		if (m_Body)
		{
			m_Body->SetAwake(value);
		}
		else
		{
			m_BodyDefinition.awake = value;
		}
	}

	bool RigidBody::IsAwake() const
	{
		if (m_Body)
		{
			return m_Body->IsAwake();
		}
		else
		{
			return m_BodyDefinition.awake;
		}
	}

	void RigidBody::SetFixedRotation(bool value)
	{
		if (m_Body)
		{
			m_Body->SetFixedRotation(value);
		}
		else
		{
			m_BodyDefinition.fixedRotation = value;
		}
	}

	bool RigidBody::IsFixedRotation() const
	{
		if (m_Body)
		{
			return m_Body->IsFixedRotation();
		}
		else
		{
			return m_BodyDefinition.fixedRotation;
		}
		
	}

	void RigidBody::SetBullet(bool value)
	{
		if (m_Body)
		{
			m_Body->SetBullet(value);
		}
		else
		{
			m_BodyDefinition.bullet = value;
		}
		
	}

	bool RigidBody::IsBullet() const
	{
		if (m_Body)
		{
			return m_Body->IsBullet();
		}
		else
		{
			return m_BodyDefinition.bullet;
		}
	}

	void RigidBody::SetGravityScale(float scale)
	{
		if (m_Body)
		{
			m_Body->SetGravityScale(scale);
		}
		else
		{
			m_BodyDefinition.gravityScale = scale;
		}
	}

	float RigidBody::GetGravityScale() const
	{
		if (m_Body)
		{
			return m_Body->GetGravityScale();
		}
		else
		{
			return m_BodyDefinition.gravityScale;
		}
	}

	void RigidBody::SetCustomMassData(float mass, const Math::vec2 &center, float inertia)
	{
		m_UseCustomMassData = true;
		m_Mass = mass;
		m_CenterOfMass = center;
		m_Inertia = inertia;
		if (m_Body)
		{
			b2MassData b2_data;
			b2_data.mass = mass;
			b2_data.center = { center.x, center.y };
			b2_data.I = inertia;
			m_Body->SetMassData(&b2_data);
		}
	}

	void RigidBody::SetCustomMassData(MassData data)
	{
		m_UseCustomMassData = true;
		m_Mass = data.mass;
		m_CenterOfMass = data.center;
		m_Inertia = data.inertia;
		if (m_Body)
		{
			b2MassData b2_data;
			b2_data.mass = data.mass;
			b2_data.center = { data.center.x, data.center.y };
			b2_data.I = data.inertia;
			m_Body->SetMassData(&b2_data);
		}
	}

	void RigidBody::ResetMassData()
	{
		m_UseCustomMassData = false;
		if (m_Body)
			m_Body->ResetMassData();
	}

	RigidBody::MassData RigidBody::GetMassData()
	{
		if (m_Body)
		{
			b2MassData b2_data;
			m_Body->GetMassData(&b2_data);
			m_Mass = b2_data.mass;
			m_CenterOfMass = { b2_data.center.x, b2_data.center.y };
			m_Inertia = b2_data.I;
		}
		MassData data;
		data.mass = m_Mass;
		data.center = m_CenterOfMass;
		data.inertia = m_Inertia;
		return data;
	}
}

