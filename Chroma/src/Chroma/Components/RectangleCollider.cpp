#include "chromapch.h"
#include "RectangleCollider.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	void RectangleCollider::CreateReflectionModel()
	{
		Reflection::RegisterComponent<RectangleCollider, Collider>();
		Reflection::RegisterComponentProperty<RectangleCollider, static_cast<void (RectangleCollider::*)(const Math::vec2 &)>(&RectangleCollider::SetSize), &RectangleCollider::GetSize>("Size");
		Reflection::RegisterComponentProperty<RectangleCollider, &RectangleCollider::SetRotation, &RectangleCollider::GetRotation>("Rotation");
	}
	void RectangleCollider::Initialize()
	{
		m_Shape.SetAsBox(10 * Physics::GetScale(), 10 * Physics::GetScale());
	}

	Math::vec2 RectangleCollider::GetPosition() const
	{
		return Math::vec2(m_Shape.m_centroid.x / Physics::GetScale(), m_Shape.m_centroid.y / Physics::GetScale());
	}

	void RectangleCollider::SetPosition(const Math::vec2 &value)
	{
		m_Shape.m_centroid = { value.x * Physics::GetScale(), value.y * Physics::GetScale() };
	}

	void RectangleCollider::SetSize(float hx, float hy)
	{
		m_Shape.SetAsBox(hx * Physics::GetScale(), hy * Physics::GetScale(), m_Shape.m_centroid, m_Rotation);
	}

	void RectangleCollider::SetSize(const Math::vec2 &size)
	{
		m_Shape.SetAsBox(size.x * Physics::GetScale(), size.y * Physics::GetScale(), m_Shape.m_centroid, m_Rotation);
	}

	Math::vec2 RectangleCollider::GetSize() const
	{
		return Math::vec2(m_Shape.m_vertices[2].x / Physics::GetScale(), m_Shape.m_vertices[2].y / Physics::GetScale());
	}

	void RectangleCollider::SetRotation(float angle)
	{
		m_Shape.SetAsBox(m_Shape.m_vertices[2].x, m_Shape.m_vertices[2].y, m_Shape.m_centroid, angle);
		m_Rotation = angle;
	}

	float RectangleCollider::GetRotation() const
	{
		return m_Rotation;
	}

}