#include "chromapch.h"
#include "CollisionSystem.h"

#include "Chroma/Components/Transform.h"
#include "Chroma/Components/BoxCollider.h"
#include "Chroma/Components/CircleCollider.h"

#include "cute_headers/cute_c2.h"

namespace Chroma
{
	void CollisionSystem::Update(Time time)
	{
		for (auto entity : m_Scene->Registry.view<Chroma::Transform, Chroma::BoxCollider>())
		{
			auto& collider = m_Scene->Registry.get<Chroma::BoxCollider>(entity);

			auto position = m_Scene->GetTransformAbsolutePosition(entity);

			c2AABB boundingBox;
			boundingBox.min = { position.x + collider.Min.x, position.y + collider.Min.y };
			boundingBox.max = { position.x + collider.Max.x, position.y + collider.Max.y };

			bool wasColliding = collider.m_Colliding;
			bool collision = false;

			for (auto entity2 : m_Scene->Registry.view<Chroma::Transform, Chroma::BoxCollider>())
			{
				if (entity == entity2)
					continue;

				auto& collider2 = m_Scene->Registry.get<Chroma::BoxCollider>(entity2);

				auto position2 = m_Scene->GetTransformAbsolutePosition(entity2);

				c2AABB boundingBox2;
				boundingBox2.min = { position2.x + collider2.Min.x, position2.y + collider2.Min.y };
				boundingBox2.max = { position2.x + collider2.Max.x, position2.y + collider2.Max.y };


				if (c2AABBtoAABB(boundingBox, boundingBox2))
				{
					collision = true;
					break;
				}
					

			}

			collider.m_Colliding = collision;


			if (!wasColliding && collision)
			{
				//OnEnterCollision
			}

			if (wasColliding && !collision)
			{
				//OnExitCollision
			}

			if (collision)
			{
				//OnCollision
			}


		}

	}
}


