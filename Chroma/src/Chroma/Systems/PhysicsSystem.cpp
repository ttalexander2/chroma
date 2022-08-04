#include "chromapch.h"

#include "PhysicsSystem.h"

#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Collider.h>
#include "Chroma/Components/RigidBody.h"
#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Components/CSharpScript.h"
#include "Chroma/Physics/Physics.h"
#include <Chroma/Renderer/Renderer2D.h>

namespace Chroma
{

	bool PhysicsSystem::PhysicsContactFilter::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB)
	{
		Collider *collider_a = reinterpret_cast<Collider*>(fixtureA->GetUserData().pointer);
		Collider *collider_b = reinterpret_cast<Collider*>(fixtureB->GetUserData().pointer);

		CHROMA_CORE_INFO("Should Collide: {}, {}", collider_a->GetEntityID(), collider_b->GetEntityID());

		return true;

		//bool collide_a = (collider_a->Mask & collider_b->Layer) != 0;
		//bool collide_b = (collider_a->Layer & collider_b->Mask) != 0;
		//
		//return collide_a && collide_b;
	}


	void PhysicsSystem::Init()
	{
		m_World = new b2World(m_Gravity);
		m_World->SetContactFilter(&m_ContactFilter);
		m_World->SetDebugDraw(&m_DebugDraw);
		m_World->SetContactListener(dynamic_cast<b2ContactListener*>(&m_ContactFilter));

		m_ContactListener.m_Scene = m_Scene;
		m_ContactFilter.m_Scene = m_Scene;

		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		//flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_aabbBit;
		//flags += b2Draw::e_centerOfMassBit;
		m_DebugDraw.SetFlags(flags);


		auto view = m_Scene->Registry.view<Transform, RigidBody>();

		for (auto &entity : view)
		{
			auto &transform = view.get<Transform>(entity);
			auto &rigidbody = view.get<RigidBody>(entity);

			rigidbody.m_BodyDefinition.position = { transform.Position.x * Physics::GetScale(), transform.Position.y * Physics::GetScale() };
			rigidbody.m_BodyDefinition.angle = transform.Rotation;

			rigidbody.m_Body = m_World->CreateBody(&rigidbody.m_BodyDefinition);
			rigidbody.m_Body->SetType(b2BodyType::b2_dynamicBody);
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

	
	void PhysicsSystem::CreateFixture(EntityID entity, RigidBody *body)
	{
		Transform &transform = m_Scene->Registry.get<Transform>(entity);
		auto colliders = m_Scene->GetColliders(entity);

		for (Collider* collider : colliders)
		{
			collider->m_FixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(collider);
			collider->m_FixtureDef.shape = collider->GetShapeHandle();
			collider->m_FixtureDef.isSensor = false;
			collider->m_Fixture = body->m_Body->CreateFixture(&collider->m_FixtureDef);
		}

		if (transform.HasChildren())
		{
			for (auto child : transform.Children)
			{
				if (!m_Scene->HasComponent<RigidBody>(child))
					CreateFixture(child, body);
			}
		}
	}

	void PhysicsSystem::Update(Time time)
	{
		m_World->Step(time.GetSeconds(), m_VelocityIterations, m_PositionIterations);
		
		auto view = m_Scene->Registry.view<Transform, RigidBody>();
		for (auto &entity : view)
		{
			auto &transform = view.get<Transform>(entity);
			auto &rigidbody = view.get<RigidBody>(entity);

			if (rigidbody.IsEnabled())
			{
				const auto &position = rigidbody.m_Body->GetPosition();

				transform.Position = Math::vec2(position.x / Physics::GetScale(), position.y / Physics::GetScale());
				transform.Rotation = rigidbody.m_Body->GetAngle();
			}
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
			Chroma::Renderer2D::DrawLine({ p1.x / Physics::GetScale(), p1.y / Physics::GetScale() }, { p2.x / Physics::GetScale(), p2.y / Physics::GetScale() }, 0.1f, { color.r, color.g, color.b, color.a });
			p1 = p2;
		}
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color)
	{
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

		//for (int32 i = 1; i < vertexCount - 1; ++i)
		//{
		//	//Renderer2D::DrawTriangle()
		//	Math::vec2 p1 = { vertices[0].x, vertices[0].y };
		//	Math::vec2 p2 = { vertices[i].x, vertices[i].y };
		//	Math::vec2 p3 = { vertices[i + 1].x, vertices[i + 1].y };
		//	Chroma::Renderer2D::DrawTriangle(p1, p2, p3, { fillColor.r, fillColor.g, fillColor.b, fillColor.a });
		//}

		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = vertices[i];
			Chroma::Renderer2D::DrawLine({ p1.x / Physics::GetScale(), p1.y / Physics::GetScale() }, { p2.x / Physics::GetScale(), p2.y / Physics::GetScale() }, 0.1f, { color.r, color.g, color.b, color.a });
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
			Chroma::Renderer2D::DrawLine({ v1.x / Physics::GetScale(), v1.y / Physics::GetScale() }, { v2.x / Physics::GetScale(), v2.y / Physics::GetScale() }, 1, { color.r, color.g, color.b, color.a });
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
			Chroma::Renderer2D::DrawLine({ v1.x / Physics::GetScale(), v1.y / Physics::GetScale() }, { v2.x / Physics::GetScale(), v2.y / Physics::GetScale() }, 1, { color.r, color.g, color.b, color.a });
			r1 = r2;
			v1 = v2;
		}
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color)
	{
		Chroma::Renderer2D::DrawLine({ p1.x / Physics::GetScale(), p1.y / Physics::GetScale() }, { p2.x / Physics::GetScale(), p2.y / Physics::GetScale() }, 1, { color.r, color.g, color.b, color.a });

	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawTransform(const b2Transform &xf)
	{
	}

	void PhysicsSystem::PhysicsSystemDebugDraw::DrawPoint(const b2Vec2 &p, float size, const b2Color &color)
	{
	}

	void PhysicsSystem::PhysicsContactListener::BeginContact(b2Contact *contact)
	{
		b2Fixture* A = contact->GetFixtureA();
		b2Fixture* B = contact->GetFixtureB();
		Collider* colliderA = reinterpret_cast<Collider*>(A->GetUserData().pointer);
		Collider* colliderB = reinterpret_cast<Collider*>(B->GetUserData().pointer);

		CHROMA_CORE_TRACE("Begin Contact");

		MonoScripting::OnBeginContact(colliderA, colliderB, contact);

	}

	void PhysicsSystem::PhysicsContactListener::EndContact(b2Contact *contact)
	{
	}

	void PhysicsSystem::PhysicsContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
	{
	}

	void PhysicsSystem::PhysicsContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
	{
	}

} //namespace Chroma