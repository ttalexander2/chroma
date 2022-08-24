#include "chromapch.h"
#include "PolygonCollider.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	bool PolygonCollider::ValidateConvexity()
	{
		return m_Shape.Validate();
	}

	bool PolygonCollider::IsValidPolygon()
	{
		return m_Shape.m_count >= 3;
	}

	Math::vec2 PolygonCollider::GetPosition() const
	{
		return Math::vec2(m_Shape.m_centroid.x / Physics::GetScale(), m_Shape.m_centroid.y / Physics::GetScale());
	}

	void PolygonCollider::SetPosition(const Math::vec2 &value)
	{
		m_Shape.m_centroid = { value.x * Physics::GetScale(), value.y * Physics::GetScale() };
	}

	bool PolygonCollider::PushVertex(const Math::vec2 &vertex, const Math::vec2 &normal)
	{
		if (m_Shape.m_count >= b2_maxPolygonVertices)
			return false;

		m_Shape.m_vertices[m_Shape.m_count] = { vertex.x * Physics::GetScale(), vertex.y * Physics::GetScale() };
		m_Shape.m_normals[m_Shape.m_count] = { normal.x, normal.y };
		m_Shape.m_count++;
		return true;
	}

	bool PolygonCollider::PopVertex()
	{
		if (m_Shape.m_count <= 0)
			return false;

		m_Shape.m_count--;
		return true;
	}

	bool PolygonCollider::SetVertex(int32_t index, const Math::vec2 &vertex, const Math::vec2 &normal)
	{
		if (index >= b2_maxPolygonVertices)
			return false;

		if (index == m_Shape.m_count)
			m_Shape.m_count++;

		m_Shape.m_vertices[index] = { vertex.x * Physics::GetScale(), vertex.y * Physics::GetScale() };
		m_Shape.m_normals[index] = { normal.x, normal.y };
		return true;
	}

	bool PolygonCollider::RemoveVertex(int32_t index)
	{
		if (m_Shape.m_count < index)
			return false;

		if (m_Shape.m_count - 1 == index)
		{
			m_Shape.m_count--;
			return true;
		}

		for (int i = index; i < b2_maxPolygonVertices - 1; i++)
		{
			m_Shape.m_vertices[i] = m_Shape.m_vertices[i + 1];
			m_Shape.m_normals[i] = m_Shape.m_normals[i + 1];
		}
		m_Shape.m_count--;
		return true;
	}

	std::vector<PolygonCollider::Vertex> PolygonCollider::GetVertices()
	{
		std::vector<Vertex> vertices;
		for (int i = 0; i < m_Shape.m_count; i++)
		{
			Vertex v{};
			v.vertex = { m_Shape.m_vertices[i].x / Physics::GetScale(), m_Shape.m_vertices[i].y / Physics::GetScale() };
			v.normal = { m_Shape.m_normals[i].x, m_Shape.m_normals[i].y };
			vertices.push_back(v);
		}
		return vertices;
	}

	int32_t PolygonCollider::Count()
	{
		return m_Shape.m_count;
	}

	bool PolygonCollider::GetVertex(PolygonCollider::Vertex *out, int32_t index)
	{
		if (index >= m_Shape.m_count)
			return false;

		out->vertex = { m_Shape.m_vertices[index].x / Physics::GetScale(), m_Shape.m_vertices[index].y / Physics::GetScale() };
		out->normal = { m_Shape.m_normals[index].x, m_Shape.m_normals[index].y };
		return true;
	}

	Reflection::TypeFactory<PolygonCollider> PolygonCollider::RegisterType()
	{
		return Reflection::Register<PolygonCollider>("PolygonCollider")
				.Data<&PolygonCollider::SetVertices, &PolygonCollider::GetVertices>("Vertices");
	}

	void PolygonCollider::SetVertices(const std::vector<Vertex> &vertices)
	{
		int i = 0;
		for (const Vertex& v : vertices)
		{
			if (i >= MaxVertices)
				break;

			m_Shape.m_vertices[i] = b2Vec2(v.vertex.x * Physics::GetScale(), v.vertex.y * Physics::GetScale());
			m_Shape.m_normals[i] = b2Vec2(v.normal.x * Physics::GetScale(), v.normal.y * Physics::GetScale());

			i++;
		}
	}

}