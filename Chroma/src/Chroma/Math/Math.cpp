#include "chromapch.h"
#include "Math.h"

namespace glm
{
	// Normalizes any number to an arbitrary range 
	// by assuming the range wraps around when going below min or above max 
	float normalize(const float value, const float start, const float end)
	{
		const float width = end - start; // 
		const float offsetValue = value - start; // value relative to 0

		return (offsetValue - (floor(offsetValue / width) * width)) + start;
		// + start to reset back to start of original range
	}
}
