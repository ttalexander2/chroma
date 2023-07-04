#include "chromapch.h"
#include "RigidBody.h"

#include "Chroma/Reflection/Reflection.h"
#include "Chroma/Physics/Physics.h"

namespace Chroma
{
	void RigidBody::Initialize()
	{
		m_BodyDefinition.userData.pointer = reinterpret_cast<uintptr_t>(&m_EntityID);
		m_BodyDefinition.type = b2_dynamicBody;
		m_BodyDefinition.allowSleep = false;
		m_BodyDefinition.angle = 0;
		m_BodyDefinition.bullet = false;
		m_BodyDefinition.enabled = true;
		m_BodyDefinition.fixedRotation = false;
		m_BodyDefinition.gravityScale = 1.f;
		m_BodyDefinition.linearDamping = 0.f;
		m_BodyDefinition.angularDamping = 0.f;
	}

	const void RigidBody::SetBodyType(BodyType type)
	{
		switch (type)
		{
			case BodyType::Dynamic:
				m_BodyDefinition.type = b2_dynamicBody;
				break;
			case BodyType::Kinematic:
				m_BodyDefinition.type = b2_kinematicBody;
				break;
			case BodyType::Static:
				m_BodyDefinition.type = b2_staticBody;
				break;
		}
	}

	RigidBody::BodyType RigidBody::GetBodyType() const
	{
		switch (m_BodyDefinition.type)
		{
			case b2_dynamicBody:
				return BodyType::Dynamic;
			case b2_kinematicBody:
				return BodyType::Kinematic;
			case b2_staticBody:
				return BodyType::Static;
		}
		return BodyType::Static;
	}

	void RigidBody::SetLinearVelocity(const Math::vec2 &velocity)
	{
		if (m_Body)
		{
			m_Body->SetLinearVelocity({ velocity.x * Physics::GetScale(), velocity.y * Physics::GetScale() });
		}
		else
		{
			m_BodyDefinition.linearVelocity = { velocity.x * Physics::GetScale(), velocity.y * Physics::GetScale() };
		}
	}

	Math::vec2 RigidBody::GetLinearVelocity() const
	{
		if (m_Body)
		{
			const b2Vec2 v = m_Body->GetLinearVelocity();
			return { v.x / Physics::GetScale(), v.y / Physics::GetScale() };
		}
		return { m_BodyDefinition.linearVelocity.x / Physics::GetScale(), m_BodyDefinition.linearVelocity.y / Physics::GetScale() };
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
		return m_BodyDefinition.angularVelocity;
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
		return m_BodyDefinition.linearDamping;
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
		return m_BodyDefinition.angularDamping;
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
		return m_BodyDefinition.allowSleep;
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
		return m_BodyDefinition.awake;
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
		return m_BodyDefinition.fixedRotation;
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
		return m_BodyDefinition.bullet;
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
		return m_BodyDefinition.gravityScale;
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

	void RigidBody::ApplyForce(const Math::vec2 &force, const Math::vec2 &point)
	{
		ApplyForce(force, point, true);
	}

	void RigidBody::ApplyForce(const Math::vec2 &force, const Math::vec2 &point, bool wake)
	{
		if (m_Body)
			m_Body->ApplyForce(b2Vec2(force.x * Physics::GetScale(), force.y * Physics::GetScale()), b2Vec2(point.x * Physics::GetScale(), point.y * Physics::GetScale()), wake);
	}

	void RigidBody::ApplyForce(const Math::vec2 &force)
	{
		ApplyForce(force, true);
	}

	void RigidBody::ApplyForce(const Math::vec2 &force, bool wake)
	{
		if (m_Body)
			m_Body->ApplyForceToCenter(b2Vec2(force.x * Physics::GetScale(), force.y * Physics::GetScale()), wake);
	}

	void RigidBody::ApplyTorque(float torque)
	{
		ApplyTorque(torque, true);
	}

	void RigidBody::ApplyTorque(float torque, bool wake)
	{
		if (m_Body)
			m_Body->ApplyTorque(torque, wake);
	}

	void RigidBody::ApplyLinearImpulse(const Math::vec2 &impulse, const Math::vec2 &point)
	{
		ApplyLinearImpulse(impulse, point, true);
	}

	void RigidBody::ApplyLinearImpulse(const Math::vec2 &impulse, const Math::vec2 &point, bool wake)
	{
		if (m_Body)
			m_Body->ApplyLinearImpulse(b2Vec2(impulse.x * Physics::GetScale(), impulse.y * Physics::GetScale()), b2Vec2(point.x * Physics::GetScale(), point.y * Physics::GetScale()), wake);
	}

	void RigidBody::ApplyLinearImpulse(const Math::vec2 &impulse)
	{
		ApplyLinearImpulse(impulse, true);
	}

	void RigidBody::ApplyLinearImpulse(const Math::vec2 &impulse, bool wake)
	{
		if (m_Body)
			m_Body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x * Physics::GetScale(), impulse.y * Physics::GetScale()), wake);
	}

	void RigidBody::ApplyAngularImpulse(float impulse)
	{
		ApplyAngularImpulse(impulse, true);
	}

	void RigidBody::ApplyAngularImpulse(float impulse, bool wake)
	{
		if (m_Body)
			m_Body->ApplyAngularImpulse(impulse, wake);
	}


	RigidBody::TypeFactory RigidBody::register_type()
	{
		using namespace Reflection;
		
		Reflection::register_type<MassData>("RigidBody::MassData")
				.data<&MassData::mass>("mass")
				.data<&MassData::center>("center")
				.data<&MassData::inertia>("inertia");

		Reflection::register_type<BodyType>("RigidBody::BodyType")
				.data<BodyType::Static>("Static")
				.data<BodyType::Kinematic>("Kinematic")
				.data<BodyType::Dynamic>("Dynamic");

		return Reflection::RegisterComponent<RigidBody>("RigidBody")
		       .base<Component>()
		       .data<&RigidBody::GetBodyType, &RigidBody::SetBodyType>("BodyType")
		       .data<&RigidBody::GetLinearVelocity, &RigidBody::SetLinearVelocity>("LinearVelocity")
		       .data<&RigidBody::GetAngularVelocity, &RigidBody::SetAngularVelocity>("AngularVelocity")
		       .data<&RigidBody::GetLinearDamping, &RigidBody::SetLinearDamping>("LinearDamping")
		       .data<&RigidBody::GetAngularDamping, &RigidBody::SetAngularDamping>("AngularDamping")
		       .data<&RigidBody::IsSleepingAllowed, &RigidBody::SetSleepingAllowed>("SleepingAllowed")
		       .data<&RigidBody::IsAwake, &RigidBody::SetAwake>("Awake")
		       .data<&RigidBody::IsFixedRotation, &RigidBody::SetFixedRotation>("FixedRotation")
		       .data<&RigidBody::IsBullet, &RigidBody::SetBullet>("Bullet")
		       .data<&RigidBody::GetGravityScale, &RigidBody::SetGravityScale>("GravityScale")
		       .data<&RigidBody::HasCustomMassData, nullptr>("UsesCustomMassData")
		       .data<&RigidBody::GetMassData, static_cast<void (RigidBody::*)(MassData)>(&RigidBody::SetCustomMassData)>("MassData")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &, const Math::vec2 &)>(&RigidBody::ApplyForce)>("ApplyForce")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &, const Math::vec2 &, bool)>(&RigidBody::ApplyForce)>("ApplyForce")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &)>(&RigidBody::ApplyForce)>("ApplyForce")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &, bool)>(&RigidBody::ApplyForce)>("ApplyForce")
		       .function<static_cast<void (RigidBody::*)(float)>(&RigidBody::ApplyTorque)>("ApplyTorque")
		       .function<static_cast<void (RigidBody::*)(float, bool)>(&RigidBody::ApplyTorque)>("ApplyTorque")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &, const Math::vec2 &)>(&RigidBody::ApplyLinearImpulse)>("ApplyLinearImpulse")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &, const Math::vec2 &, bool)>(&RigidBody::ApplyLinearImpulse)>("ApplyLinearImpulse")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &, bool)>(&RigidBody::ApplyLinearImpulse)>("ApplyLinearImpulse")
		       .function<static_cast<void (RigidBody::*)(const Math::vec2 &)>(&RigidBody::ApplyLinearImpulse)>("ApplyLinearImpulse")
		       .function<static_cast<void (RigidBody::*)(float)>(&RigidBody::ApplyAngularImpulse)>("ApplyAngularImpulse")
		       .function<static_cast<void (RigidBody::*)(float, bool)>(&RigidBody::ApplyAngularImpulse)>("ApplyAngularImpulse");
	}
}
