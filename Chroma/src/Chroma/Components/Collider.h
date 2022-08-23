#pragma once

#include "Chroma/Scene/Component.h"

#include <box2d.h>
#include "Chroma/Physics/Physics.h"

namespace Chroma
{

	struct Collider : public Component
	{
		CHROMA_ABSTRACT_COMPONENT(Collider, Component);
		friend class PhysicsSystem;

		enum class ColliderType : char
		{
			Circle = 0,
			Edge = 1,
			Polygon = 2,
			Rectangle = 3
		};

		inline virtual const ColliderType GetColliderType() const { return ColliderType::Rectangle; }
		void Initialize() override;

		struct Vertex
		{
			Math::vec2 vertex;
			Math::vec2 normal;
		};

		uint32_t Mask = 1;
		uint32_t Layer = 1;


		virtual Math::vec2 GetPosition() const = 0;
		virtual void SetPosition(const Math::vec2 &value) = 0;

		inline virtual b2Shape *GetShapeHandle() = 0;

		void SetFriction(float value);
		float GetFriction() const;

		void SetRestitution(float value);
		float GetRestitution() const;

		void SetRestitutionThreshold(float value);
		float GetRestitutionThreshold() const;

		void SetDensity(float density);
		float GetDensity() const;

		void SetSensor(bool sensor);
		bool IsSensor() const;

	private:
		b2FixtureDef m_FixtureDef;
		b2Fixture *m_Fixture = nullptr;
	};


}