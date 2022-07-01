#include "chromapch.h"
#include "Collider.h"

#include "Chroma/Reflection/Reflection.h"


namespace Chroma
{
	void Collider::CreateReflectionModel()
	{
		Reflection::RegisterComponent<Collider, Component>();
		//Reflection::RegisterComponentProperty<Collider, 
	}

	void Collider::Initialize()
	{
	}


	void Collider::SetFriction(float value)
	{
		if (m_Fixture)
		{
			m_Fixture->SetFriction(value);
		}
		else
		{
			m_FixtureDef.friction = value;
		}

	}

	float Collider::GetFriction() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetFriction();
		}
		else
		{
			return m_FixtureDef.friction;
		}
	}

	void Collider::SetRestitution(float value)
	{
		if (m_Fixture)
		{
			m_Fixture->SetRestitution(value);
		}
		else
		{
			m_FixtureDef.restitution = value;
		}
	}

	float Collider::GetRestitution() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetRestitution();
		}
		else
		{
			return m_FixtureDef.restitution;
		}
	}

	void Collider::SetRestitutionThreshold(float value)
	{
		if (m_Fixture)
		{
			m_Fixture->SetRestitutionThreshold(value);
		}
		else
		{
			m_FixtureDef.restitutionThreshold = value;
		}
	}

	float Collider::GetRestitutionThreshold() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetRestitutionThreshold();
		}
		else
		{
			return m_FixtureDef.restitutionThreshold;
		}
	}

	void Collider::SetDensity(float density)
	{
		if (m_Fixture)
		{
			m_Fixture->SetDensity(density);
		}
		else
		{
			m_FixtureDef.density = density;
		}
	}

	float Collider::GetDensity() const
	{
		if (m_Fixture)
		{
			return m_Fixture->GetDensity();
		}
		else
		{
			return m_FixtureDef.density;
		}
	}

	void Collider::SetSensor(bool sensor)
	{
		if (m_Fixture)
		{
			m_Fixture->SetSensor(sensor);
		}
		else
		{
			m_FixtureDef.isSensor = sensor;
		}
	}

	bool Collider::IsSensor() const
	{
		if (m_Fixture)
		{
			return m_Fixture->IsSensor();
		}
		else
		{
			return m_FixtureDef.isSensor;
		}
	}

	void Collider::SetShapeType(ShapeType type)
	{
		if (m_Shape)
			delete m_Shape;
		switch (type)
		{
			case ShapeType::Circle:
				m_Shape = new Circle();
				break;
			case ShapeType::Chain:
				m_Shape = new Chain();
				break;
			case ShapeType::Edge:
				m_Shape = new Edge();
				break;
			case ShapeType::Polygon:
				m_Shape = new Polygon();
				break;
			case ShapeType::Rectangle:
				m_Shape = new Rectangle();
				break;
		}
	}

	float Collider::Circle::GetRadius() const
	{
		return m_Shape.m_radius;
	}

	void Collider::Circle::SetRadius(float value)
	{
		m_Shape.m_radius = value;
	}

	Math::vec2 Collider::Circle::GetPosition() const
	{
		return Math::vec2(m_Shape.m_p.x, m_Shape.m_p.y);
	}

	void Collider::Circle::SetPosition(const Math::vec2 &value)
	{
		m_Shape.m_p = { value.x, value.y };
	}

	bool Collider::Polygon::ValidateConvexity()
	{
		return m_Shape.Validate();
	}

	bool Collider::Polygon::IsValidPolygon()
	{
		return m_Shape.m_count >= 3;
	}

	Math::vec2 Collider::Polygon::GetPosition() const
	{
		return Math::vec2(m_Shape.m_centroid.x, m_Shape.m_centroid.y);
	}

	void Collider::Polygon::SetPosition(const Math::vec2 &value)
	{
		m_Shape.m_centroid = { value.x, value.y };
	}

	bool Collider::Polygon::PushVertex(const Math::vec2 &vertex, const Math::vec2& normal)
	{
		if (m_Shape.m_count >= b2_maxPolygonVertices)
			return false;

		m_Shape.m_vertices[m_Shape.m_count] = { vertex.x, vertex.y };
		m_Shape.m_normals[m_Shape.m_count] = { normal.x, normal.y };
		m_Shape.m_count++;
		return true;
	}

	bool Collider::Polygon::PopVertex()
	{
		if (m_Shape.m_count <= 0)
			return false;

		m_Shape.m_count--;
		return true;
	}

	bool Collider::Polygon::SetVertex(int32_t index, const Math::vec2 &vertex, const Math::vec2& normal)
	{
		if (index >= b2_maxPolygonVertices)
			return false;

		if (index == m_Shape.m_count)
			m_Shape.m_count++;

		m_Shape.m_vertices[index] = { vertex.x, vertex.y };
		m_Shape.m_normals[index] = { normal.x, normal.y };
		return true;
	}

	bool Collider::Polygon::RemoveVertex(int32_t index)
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

	std::vector<Collider::Polygon::Vertex> Collider::Polygon::GetVertices()
	{
		std::vector<Vertex> vertices;
		for (int i = 0; i < m_Shape.m_count; i++)
		{
			Vertex v{};
			v.vertex = { m_Shape.m_vertices[i].x, m_Shape.m_vertices[i].y };
			v.normal = { m_Shape.m_normals[i].x, m_Shape.m_normals[i].y };
			vertices.push_back(v);
		}
		return vertices;
	}

	int32_t Collider::Polygon::Count()
	{
		return m_Shape.m_count;
	}

	bool Collider::Polygon::GetVertex(Collider::Polygon::Vertex *out, int32_t index)
	{
		if (index >= m_Shape.m_count)
			return false;

		out->vertex = { m_Shape.m_vertices[index].x, m_Shape.m_vertices[index].y };
		out->normal = { m_Shape.m_normals[index].x, m_Shape.m_normals[index].y };
		return true;
	}


	Math::vec2 Collider::Edge::GetPosition() const
	{
		return Math::vec2(m_Shape.m_vertex2.x, m_Shape.m_vertex2.y);
	}

	void Collider::Edge::SetPosition(const Math::vec2 &value)
	{
		b2Vec2 currentOrigin = m_Shape.m_vertex2;
		m_Shape.m_vertex0 = { m_Shape.m_vertex0.x - currentOrigin.x + value.x, m_Shape.m_vertex0.y - currentOrigin.y + value.y };
		m_Shape.m_vertex1 = { m_Shape.m_vertex1.x - currentOrigin.x + value.x, m_Shape.m_vertex1.y - currentOrigin.y + value.y };
		m_Shape.m_vertex2 = { m_Shape.m_vertex2.x - currentOrigin.x + value.x, m_Shape.m_vertex2.y - currentOrigin.y + value.y };
		m_Shape.m_vertex3 = { m_Shape.m_vertex3.x - currentOrigin.x + value.x, m_Shape.m_vertex3.y - currentOrigin.y + value.y };
	}

	void Collider::Edge::SetOneSided(bool val)
	{
		m_Shape.m_oneSided = val;
	}

	bool Collider::Edge::IsOneSided() const
	{
		return m_Shape.m_oneSided;
	}

	void Collider::Edge::SetV0(const Math::vec2 &val)
	{
		m_Shape.m_vertex0 = { val.x, val.y };
	}

	void Collider::Edge::SetV1(const Math::vec2 &val)
	{
		m_Shape.m_vertex1 = { val.x, val.y };
	}

	void Collider::Edge::SetV2(const Math::vec2 &val)
	{
		m_Shape.m_vertex2 = { val.x, val.y };
	}

	void Collider::Edge::SetV3(const Math::vec2 &val)
	{
		m_Shape.m_vertex3 = { val.x, val.y };
	}

	Math::vec2 Collider::Edge::GetV0() const
	{
		return Math::vec2(m_Shape.m_vertex0.x, m_Shape.m_vertex0.y);
	}

	Math::vec2 Collider::Edge::GetV1() const
	{
		return Math::vec2(m_Shape.m_vertex1.x, m_Shape.m_vertex1.y);
	}

	Math::vec2 Collider::Edge::GetV2() const
	{
		return Math::vec2(m_Shape.m_vertex2.x, m_Shape.m_vertex2.y);
	}

	Math::vec2 Collider::Edge::GetV3() const
	{
		return Math::vec2(m_Shape.m_vertex3.x, m_Shape.m_vertex3.y);
	}

} //namespace Chroma