#include "chromapch.h"
#include "Frustum.h"

#include "Chroma/Math/Geometric.h"
#include <glm/gtx/string_cast.hpp>


// Adapted from https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644
// See https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
// for info on extraction of clipping planes.
// See https://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
// for info on handling large objects behind camera intersecting the near plane.


namespace Chroma
{
	Frustum::Frustum(Math::mat4 viewProjMat)
	{

		viewProjMat = glm::transpose(viewProjMat);

		m_Planes[Left]		= viewProjMat[3] + viewProjMat[0];
		m_Planes[Right]		= viewProjMat[3] - viewProjMat[0];
		m_Planes[Bottom]	= viewProjMat[3] + viewProjMat[1];
		m_Planes[Top]		= viewProjMat[3] - viewProjMat[1];
		m_Planes[Near]		= viewProjMat[3] + viewProjMat[2];
		m_Planes[Far]		= viewProjMat[3] - viewProjMat[2];

		//for (int i = 0; i < Count; i++)
			//m_Planes[i] = Math::normalize(m_Planes[i]);

		//Create combination of plane intersection points
		glm::vec3 crosses[Combinations] = {
			glm::cross(glm::vec3(m_Planes[Left]),   glm::vec3(m_Planes[Right])),
			glm::cross(glm::vec3(m_Planes[Left]),   glm::vec3(m_Planes[Bottom])),
			glm::cross(glm::vec3(m_Planes[Left]),   glm::vec3(m_Planes[Top])),
			glm::cross(glm::vec3(m_Planes[Left]),   glm::vec3(m_Planes[Near])),
			glm::cross(glm::vec3(m_Planes[Left]),   glm::vec3(m_Planes[Far])),
			glm::cross(glm::vec3(m_Planes[Right]),  glm::vec3(m_Planes[Bottom])),
			glm::cross(glm::vec3(m_Planes[Right]),  glm::vec3(m_Planes[Top])),
			glm::cross(glm::vec3(m_Planes[Right]),  glm::vec3(m_Planes[Near])),
			glm::cross(glm::vec3(m_Planes[Right]),  glm::vec3(m_Planes[Far])),
			glm::cross(glm::vec3(m_Planes[Bottom]), glm::vec3(m_Planes[Top])),
			glm::cross(glm::vec3(m_Planes[Bottom]), glm::vec3(m_Planes[Near])),
			glm::cross(glm::vec3(m_Planes[Bottom]), glm::vec3(m_Planes[Far])),
			glm::cross(glm::vec3(m_Planes[Top]),    glm::vec3(m_Planes[Near])),
			glm::cross(glm::vec3(m_Planes[Top]),    glm::vec3(m_Planes[Far])),
			glm::cross(glm::vec3(m_Planes[Near]),   glm::vec3(m_Planes[Far]))
		};

		m_Points[0] = Frustum::Intersection<Left, Bottom, Near>(crosses);
		m_Points[1] = Frustum::Intersection<Left, Top, Near>(crosses);
		m_Points[2] = Frustum::Intersection<Right, Bottom, Near>(crosses);
		m_Points[3] = Frustum::Intersection<Right, Top, Near>(crosses);
		m_Points[4] = Frustum::Intersection<Left, Bottom, Far>(crosses);
		m_Points[5] = Frustum::Intersection<Left, Top, Far>(crosses);
		m_Points[6] = Frustum::Intersection<Right, Bottom, Far>(crosses);
		m_Points[7] = Frustum::Intersection<Right, Top, Far>(crosses);

	}
	bool Frustum::SphereIntersects(const Math::vec3& center, float radius)
	{
		// Check point for each plane, calculate (center * normal) + distance + radius
		for (int i = 0; i < Count; i++)
		{
			//(p^*n^) + d = c
			if (glm::dot(center, { m_Planes[i].x, m_Planes[i].y, m_Planes[i].z }) + m_Planes[i].w + radius <= 0)
				return false;
			return true;
		}
	}
	bool Frustum::CubeIntersects(const Math::vec3& min, const Math::vec3& max)
	{
		//CHROMA_CORE_WARN("Min: {0}, Max: {1}", glm::to_string(min), glm::to_string(max));
		// Check if cube is outside
		for (int i = 0; i < Count; i++)
		{
			if ((glm::dot(m_Planes[i], { min, 1.0f }) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(max.x, min.y, min.z, 1.0f)) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(min.x, max.y, min.z, 1.0f)) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(max.x, max.y, min.z, 1.0f)) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(min.x, min.y, max.z, 1.0f)) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(max.x, min.y, max.z, 1.0f)) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(min.x, max.y, max.z, 1.0f)) < 0.0) &&
				(glm::dot(m_Planes[i], glm::vec4(max.x, max.y, max.z, 1.0f)) < 0.0))
				return false;
		}

		int out;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_Points[i].x > max.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_Points[i].x < min.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_Points[i].y > max.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_Points[i].y < min.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_Points[i].z > max.z) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_Points[i].z < min.z) ? 1 : 0); if (out == 8) return false;

		return true;

	}

	template<Frustum::Plane a, Frustum::Plane b, Frustum::Plane c>
	inline glm::vec3 Frustum::Intersection(const glm::vec3* crosses) const
	{
		float D = glm::dot(glm::vec3(m_Planes[a]), crosses[ij2k<b, c>::k]);
		glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
			glm::vec3(m_Planes[a].w, m_Planes[b].w, m_Planes[c].w);
		return res * (-1.0f / D);
	}
}