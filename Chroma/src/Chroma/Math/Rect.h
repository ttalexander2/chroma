#pragma once

#include "./Vec4.h"

namespace Math = glm;

namespace glm
{
	struct Rect : public vec4
	{
		union
		{
			struct
			{
				float x, y, width, height;
			};
		};


		operator vec4() const { return vec4(x, y, width, height); }
	};
}
