#pragma once

#include "Chroma/Scene/Component.h"
#include <bitset>
#include <box2d.h>

namespace Chroma
{
	class PhysicsSystem;

	struct RigidBody : public Component
	{
		CHROMA_COMPONENT(RigidBody, Component);
		friend class PhysicsSystem;

		void Initialize() override;


		enum class BodyType
		{
			STATIC,
			KINEMATIC,
			DYNAMIC
		};

		struct MassData
		{
			float mass = 0;
			Math::vec2 center = {0.f, 0.f};
			float inertia = 0;
		};

		const void SetBodyType(BodyType type);
		BodyType GetBodyType() const;


		void SetLinearVelocity(const Math::vec2 &velocity);
		Math::vec2 GetLinearVelocity() const;

		void SetAngularVelocity(float velocity);
		float GetAngularVelocity() const;

		void SetLinearDamping(float damping);
		float GetLinearDamping() const;

		void SetAngularDamping(float damping);
		float GetAngularDamping() const;

		void SetSleepingAllowed(bool value);
		bool IsSleepingAllowed() const;

		void SetAwake(bool value);
		bool IsAwake() const;

		void SetFixedRotation(bool value);
		bool IsFixedRotation() const;

		void SetBullet(bool value);
		bool IsBullet() const;

		void SetGravityScale(float scale);
		float GetGravityScale() const;

		void SetCustomMassData(float mass, const Math::vec2 &center, float inertia);
		void SetCustomMassData(MassData data);
		void ResetMassData();
		MassData GetMassData();


	private:

		b2BodyDef m_BodyDefinition;
		b2Body * m_Body = nullptr;
		std::vector<b2FixtureDef> m_FixtureDefinitions;
		std::vector<b2Fixture> m_Fixtures;

		bool m_UseCustomMassData = false;
		float m_Mass = 0;
		Math::vec2 m_CenterOfMass = { 0.f, 0.f };
		float m_Inertia = 0;



	};
}