#include "chromapch.h"
#include "Frustum.h"

#include "Chroma/Math/Geometric.h"
#include <glm/gtx/string_cast.hpp>


// Adapted from https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644
// See https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
// for info on extraction of clipping planes.


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
		m_Planes[Far]		= viewProjMat[3] + viewProjMat[2];

		for (int i = 0; i < Count; i++)
			m_Planes[i] = Math::normalize(m_Planes[i]);

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
		return true;

	}

}