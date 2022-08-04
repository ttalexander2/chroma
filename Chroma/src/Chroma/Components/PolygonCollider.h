#pragma once

#include "Collider.h"

namespace Chroma
{
	
	struct PolygonCollider : public Collider
	{
		CHROMA_COMPONENT(PolygonCollider, Collider);

		//PolygonCollider &operator=(const PolygonCollider &) = default;

		inline b2Shape *GetShapeHandle() override { return &m_Shape; }

		inline const ColliderType GetColliderType() const override { return ColliderType::Edge; }

		static const size_t MaxVertices = b2_maxPolygonVertices;

		/// @brief Validates convexity. This is a very time consuming operation.
		/// @return Returns true if valid
		bool ValidateConvexity();

		/// @brief Checks to make sure the polygon has at least 3 vertices.
		/// @return Returns true if valid.
		bool IsValidPolygon();

		Math::vec2 GetPosition() const override;
		void SetPosition(const Math::vec2 &value) override;

		bool PushVertex(const Math::vec2 &vertex, const Math::vec2 &normal);
		bool PopVertex();

		bool SetVertex(int32_t index, const Math::vec2 &vertex, const Math::vec2 &normal);
		bool RemoveVertex(int32_t index);

		std::vector<Vertex> GetVertices();
		int32_t Count();
		bool GetVertex(Vertex *out, int32_t index);

	private:
		void SetVertices(const std::vector<Vertex>& vertices);
		b2PolygonShape m_Shape;
	};


}