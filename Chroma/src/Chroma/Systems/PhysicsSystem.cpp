#include "chromapch.h"

#include "PhysicsSystem.h"

#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Collider.h>
#include "Chroma/Components/RigidBody.h"
#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Components/CSharpScript.h"
#include <Chroma/Renderer/Renderer2D.h>

namespace Chroma
{

	bool PhysicsSystem::PhysicsContactFilter::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB)
	{
		Collider *collider_a = (Collider *)fixtureA->GetUserData().pointer;
		Collider *collider_b = (Collider *)fixtureB->GetUserData().pointer;

		bool collide_a = (collider_a->Mask & collider_b->Layer) != 0;
		bool collide_b = (collider_a->Layer & collider_b->Mask) != 0;
		return collide_a && collide_b;
	}


	void PhysicsSystem::Init()
	{
		m_World = new b2World(m_Gravity);
		m_World->SetContactFilter(&m_ContactFilter);
		m_World->SetContinuousPhysics(true);
		m_World->SetAllowSleeping(false);
		m_World->SetDebugDraw(&m_DebugDraw);

		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_jointBit;
		flags += b2Draw::e_aabbBit;
		flags += b2Draw::e_centerOfMassBit;
		m_DebugDraw.SetFlags(flags);


		auto view = m_Scene->Registry.view<Transform, RigidBody>();

		for (auto &entity : view)
		{
			auto &transform = view.get<Transform>(entity);
			auto &rigidbody = view.get<RigidBody>(entity);

			rigidbody.m_BodyDefinition.position = { transform.Position.x, transform.Position.y };
			rigidbody.m_BodyDefinition.angle = transform.Rotation;

			rigidbody.m_Body = m_World->CreateBody(&rigidbody.m_BodyDefinition);
			if (rigidbody.m_UseCustomMassData)
			{
				b2MassData data;
				data.mass = rigidbody.m_Mass;
				data.center = { rigidbody.m_CenterOfMass.x, rigidbody.m_CenterOfMass.y };
				data.I = rigidbody.m_Inertia;
				rigidbody.m_Body->SetMassData(&data);
			}

			CreateFixture(entity, &rigidbody);
		}
	}

	
	void PhysicsSystem::CreateFixture(EntityID entity, RigidBody *parentBody)
	{
		Transform &transform = m_Scene->Registry.get<Transform>(entity);
		Collider *collider = m_Scene->Registry.try_get<Collider>(entity);
		RigidBody *childbody = m_Scene->Registry.try_get<RigidBody>(entity);
		if (collider != nullptr)
		{
			collider->m_FixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(collider);
			if (childbody != nullptr)
				collider->m_Fixture = childbody->m_Body->CreateFixture(&collider->m_FixtureDef);
			else
				collider->m_Fixture = parentBody->m_Body->CreateFixture(&collider->m_FixtureDef);
		}

		if (transform.HasChildren())
		{
			for (auto child : transform.Children)
			{
				if (childbody)
					CreateFixture(child, childbody);
				else
					CreateFixture(child, parentBody);
			}
		}
	}

	void PhysicsSystem::Update(Time time)
	{
		m_World->SetAllowSleeping(true);
		m_World->SetWarmStarting(true);
		m_World->SetContinuousPhysics(true);
		m_World->SetSubStepping(true);

		m_World->Step(time.GetSeconds(), m_VelocityIterations, m_PositionIterations);
		
		auto view = m_Scene->Registry.view<Transform, RigidBody>();
		for (auto &entity : view)
		{
			auto &transform = view.get<Transform>(entity);
			auto &rigidbody = view.get<RigidBody>(entity);

			auto& position = rigidbody.m_Body->GetPosition();
			//CHROMA_CORE_TRACE("Rigidbody Position: {}, {}", position.x, position.y);

			transform.Position = Math::vec2(position.x, position.y);
			transform.Rotation = rigidbody.m_Body->GetAngle();
		}

	}

	void PhysicsSystem::LateDraw(Time time)
	{
		if (m_World)
			m_World->DebugDraw();
	}


	PhysicsSystem::~PhysicsSystem()
	{
		delete m_World;
	}


	void PhysicsSystem::PhysicsSystemDebugDraw::DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color)
	{
		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = vertices[i];
			Chroma::Renderer2D::DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, 1, {color.r, color.g, color.b, color.a});
			p1 = p2;
		}
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color)
	{
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

		for (int32 i = 1; i < vertexCount - 1; ++i)
		{
			//Renderer2D::DrawTriangle()
			Math::vec2 p1 = { vertices[0].x, vertices[0].y };
			Math::vec2 p2 = { vertices[i].x, vertices[i].y };
			Math::vec2 p3 = { vertices[i + 1].x, vertices[i + 1].y };
			Chroma::Renderer2D::DrawTriangle(p1, p2, p3, { fillColor.r, fillColor.g, fillColor.b, fillColor.a });
		}

		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = vertices[i];
			Chroma::Renderer2D::DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, 1, { color.r, color.g, color.b, color.a });
			p1 = p2;
		}
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawCircle(const b2Vec2 &center, float radius, const b2Color &color)
	{
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			Chroma::Renderer2D::DrawLine({ v1.x, v1.y }, { v2.x, v2.y }, 1, { color.r, color.g, color.b, color.a });
			r1 = r2;
			v1 = v2;
		}

	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawSolidCircle(const b2Vec2 &center, float radius, const b2Vec2 &axis, const b2Color &color)
	{
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			Chroma::Renderer2D::DrawLine({ v1.x, v1.y }, { v2.x, v2.y }, 1, { color.r, color.g, color.b, color.a });
			r1 = r2;
			v1 = v2;
		}
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color)
	{
		Chroma::Renderer2D::DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, 1, { color.r, color.g, color.b, color.a });

	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawTransform(const b2Transform &xf)
	{
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawPoint(const b2Vec2 &p, float size, const b2Color &color)
	{
	}

} //namespace Chroma