#pragma once


#include <Chroma/Math/Math.h>

namespace Chroma
{

	class Camera
	{
	public:
		Camera(const Math::mat4& projection)
			: m_Projection(projection)
		{
		}

		const Math::mat4& GetProjection() const { return m_Projection; }

	private:
		Math::mat4 m_Projection;
	};
}