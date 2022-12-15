#pragma once

#include "Chroma/Scene/System.h"
#include "Chroma/Components/Transform.h"
#include "Chroma/Components/RigidBody.h"
#include <box2d.h>
#include <queue>


namespace Chroma
{
	class PhysicsSystem final : public System
	{
	public:
		~PhysicsSystem() override;
		void Init() override;
		void Update(Time time) override;
		void LateDraw(Time time) override;

		void SetGravity(const Math::vec2 &value) { m_Gravity = { value.x, value.y }; }
		Math::vec2 GetGravity() const { return { m_Gravity.x, m_Gravity.y }; }

		void SetVelocityIterations(int32_t value) { m_VelocityIterations = value; }
		int32_t GetVelocityIterations() const { return m_VelocityIterations; }

		void SetPositionIterations(int32_t value) { m_PositionIterations = value; }
		int32_t GetPositionIterations() const { return m_PositionIterations; }

	private:
		class PhysicsContactFilter : public b2ContactFilter
		{
		public:
			bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override;
		private:
			Scene *m_Scene;
			friend class PhysicsSystem;
		};

		class PhysicsContactListener : public b2ContactListener
		{
		public:
			void BeginContact(b2Contact *contact) override;
			void EndContact(b2Contact *contact) override;
			void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
			void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;

		private:
			Scene *m_Scene;
			friend class PhysicsSystem;
		};

		class PhysicsSystemDebugDraw : public b2Draw
		{
		public:
			void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override;
			void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override;
			void DrawCircle(const b2Vec2 &center, float radius, const b2Color &color) override;
			void DrawSolidCircle(const b2Vec2 &center, float radius, const b2Vec2 &axis, const b2Color &color) override;
			void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) override;
			void DrawTransform(const b2Transform &xf) override;
			void DrawPoint(const b2Vec2 &p, float size, const b2Color &color) override;
			//std::queue<std::function<void()>> m_DebugDrawQueue;
		};

		void CreateFixture(EntityID entity, RigidBody *rigidBody);

		b2World *m_World = nullptr;
		b2Vec2 m_Gravity = { 0.f, 0.f };
		PhysicsContactFilter m_ContactFilter = PhysicsContactFilter();
		PhysicsContactListener m_ContactListener = PhysicsContactListener();

		int32_t m_VelocityIterations = 8;
		int32_t m_PositionIterations = 3;

		PhysicsSystemDebugDraw m_DebugDraw;
	};
}
