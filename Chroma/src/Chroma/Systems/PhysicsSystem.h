#pragma once

#include "Chroma/Scene/System.h"
#include "Chroma/Components/Transform.h"
#include "Chroma/Components/RigidBody.h"
#include <box2d.h>
#include <queue>



namespace Chroma
{

	class PhysicsSystem : public System
	{
	public:
		~PhysicsSystem();
		void Init() override;
		void Update(Time time) override;
		void LateDraw(Time time) override;

		inline void SetGravity(const Math::vec2 &value) { m_Gravity = { value.x, value.y }; }
		inline Math::vec2 GetGravity() const { return {m_Gravity.x, m_Gravity.y}; }

		inline void SetVelocityIterations(int32_t value) { m_VelocityIterations = value; }
		inline int32_t GetVelocityIterations() const { return m_VelocityIterations; }

		inline void SetPositionIterations(int32_t value) { m_PositionIterations = value; }
		inline int32_t GetPositionIterations() const { return m_PositionIterations; }

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
			void BeginContact(b2Contact *contact);
			void EndContact(b2Contact *contact);
			void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
			void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);

		private:
			Scene *m_Scene;
			friend class PhysicsSystem;
		};

		class PhysicsSystemDebugDraw : public b2Draw
		{
		public:
			virtual void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override;
			virtual void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override;
			virtual void DrawCircle(const b2Vec2 &center, float radius, const b2Color &color) override;
			virtual void DrawSolidCircle(const b2Vec2 &center, float radius, const b2Vec2 &axis, const b2Color &color) override;
			virtual void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) override;
			virtual void DrawTransform(const b2Transform &xf) override;
			virtual void DrawPoint(const b2Vec2 &p, float size, const b2Color &color) override;
			//std::queue<std::function<void()>> m_DebugDrawQueue;
		};

		void CreateFixture(EntityID entity, RigidBody* rigidBody);

		b2World *m_World;
		b2Vec2 m_Gravity = { 0.f, 0.f };
		PhysicsContactFilter m_ContactFilter;
		PhysicsContactListener m_ContactListener;

		int32_t m_VelocityIterations = 8;
		int32_t m_PositionIterations = 3;

		PhysicsSystemDebugDraw m_DebugDraw;

	
	};
}