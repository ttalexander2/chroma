#pragma once

#include "glm/ext/vector_int4.hpp"
#include "Chroma/Math/Vec4.h"

struct RectI : public glm::ivec4
{
	union
	{
		struct
		{
			int x, y, width, height;
		};

	};


	operator glm::ivec4() const { return glm::ivec4(x, y, width, height); }
	operator Math::vec4() const { return Math::vec4(x, y, width, height); }
};


