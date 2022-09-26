#pragma once

#include "Chroma/Math/Mat4x4.h"

namespace Chroma
{
	class Frustum
	{
	public:
		Frustum()
		{
		}

		Frustum(Math::mat4 viewProjMat);

		bool SphereIntersects(const Math::vec3 &center, float radius);
		bool CubeIntersects(const Math::vec3 &min, const Math::vec3 &max);

	private:
		enum Plane
		{
			Left,
			Right,
			Bottom,
			Top,
			Near,
			Far,
			Count,
			Combinations = Count * (Count - 1) / 2
		};

		template <Plane a, Plane b, Plane c>
		glm::vec3 Intersection(const glm::vec3 *crosses) const;

		template <Plane i, Plane j>
		struct ij2k
		{
			enum { k = i * (9 - i) / 2 + j - 1 };
		};

		glm::vec4 m_Planes[Count];
		glm::vec3 m_Points[8];
	};
}
