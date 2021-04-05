#pragma once

#include "Chroma/Math/Mat4x4.h"

namespace Chroma
{
	class Frustum
	{
	public:
		Frustum() {}
		Frustum(Math::mat4 viewProjMat);

		bool SphereIntersects(const Math::vec3& center, float radius);
		bool CubeIntersects(const Math::vec3& min, const Math::vec3& max);

	private:

		enum Plane
		{
			Left,
			Right,
			Bottom,
			Top,
			Near,
			Far,
			Count
		};


		glm::vec4 m_Planes[Count];

	};
}


