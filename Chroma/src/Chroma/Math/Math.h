#pragma once

//Literally just wrapping part of the glm library because I'm lazy

#include "glm/glm.hpp"
namespace Math = glm;

namespace glm
{
	float normalize(float val, float min, float max);
}
