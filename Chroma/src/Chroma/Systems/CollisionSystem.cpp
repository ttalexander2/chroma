#include "chromapch.h"
#include "CollisionSystem.h"

#include "Chroma/Components/Transform.h"
#include "Chroma/Components/BoxCollider.h"
#include "Chroma/Components/CircleCollider.h"

#include "box2d.h"

namespace Chroma
{

	struct QueryCallbackType
	{
		std::function<bool(int)> m_Callback;

		bool QueryCallback(int proxy_id)
		{
			return m_Callback(proxy_id);
		}

	};

	void CollisionSystem::Init()
	{
		
	}

	void CollisionSystem::Update(Time time)
	{
		b2DynamicTree tree;

		for (auto entity : m_Scene->Registry.view<Chroma::Transform, Chroma::BoxCollider>())
		{

			auto collider = m_Scene->Registry.try_get<Chroma::BoxCollider>(entity);
			if (!collider->IsEnabled())
				continue;
			auto position = m_Scene->GetTransformAbsolutePosition(entity);

			collider->m_Colliding = false;

			b2AABB aabb;
			glm::vec2 lowerBound = position + collider->Min;
			glm::vec2 upperBound = position + collider->Max;
			aabb.lowerBound = b2Vec2(lowerBound.x, lowerBound.y);
			aabb.upperBound = b2Vec2(upperBound.x, upperBound.y);
			
			collider->m_Proxy = tree.CreateProxy(aabb, (void*)collider);

		}

		for (auto entity : m_Scene->Registry.view<Chroma::Transform, Chroma::BoxCollider>())
		{
			auto& collider = m_Scene->Registry.get<Chroma::BoxCollider>(entity);
			if (!collider.IsEnabled())
				continue;
			auto position = m_Scene->GetTransformAbsolutePosition(entity);

			b2AABB aabb;
			glm::vec2 lowerBound = position + collider.Min;
			glm::vec2 upperBound = position + collider.Max;
			aabb.lowerBound = b2Vec2(lowerBound.x, lowerBound.y);
			aabb.upperBound = b2Vec2(upperBound.x, upperBound.y);

			std::function<bool(int)> f = [&](int proxy_id) {
				Chroma::BoxCollider* collider2 = reinterpret_cast<Chroma::BoxCollider*>(tree.GetUserData(proxy_id));
				if (!collider2->IsEnabled())
					return true;

				if (entity == collider2->GetEntityID())
					return true;

				auto position2 = m_Scene->GetTransformAbsolutePosition(collider2->GetEntityID());


				b2AABB aabb2;
				glm::vec2 lowerBound2 = position2 + collider2->Min;
				glm::vec2 upperBound2 = position2 + collider2->Max;
				aabb2.lowerBound = b2Vec2(lowerBound2.x, lowerBound2.y);
				aabb2.upperBound = b2Vec2(upperBound2.x, upperBound2.y);

				collider.m_Colliding = b2TestOverlap(aabb, aabb2);

				return false;
			};

			QueryCallbackType callback;
			callback.m_Callback = f;
			
			tree.Query<QueryCallbackType>(&callback, aabb);
		}

	}
}


