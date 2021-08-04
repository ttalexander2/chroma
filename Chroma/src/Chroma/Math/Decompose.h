#pragma once

#include "Mat4x4.h"
#include <glm/fwd.hpp>



namespace Chroma
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
}
