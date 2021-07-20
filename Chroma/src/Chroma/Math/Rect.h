#pragma once

#include "./Vec4.h"


struct Rect : public Math::vec4
{
	union
	{
		struct
		{
			float x, y, width, height;
		};

	};


	operator Math::vec4() const { return Math::vec4(x, y, width, height); }
};


