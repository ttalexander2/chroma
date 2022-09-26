#pragma once

#include "Mat4x4.h"
#include <glm/fwd.hpp>


namespace Chroma
{
	/// @brief Function to decompose a 4x4 matrix transform into three 3-component vectors (translation, rotation, and scale).
	/// @param transform Transform to decompose.
	/// @param translation Resulting translation.
	/// @param rotation Resulting rotation.
	/// @param scale Resulting scale.
	/// @return Whether the decomposition was successful.
	bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale);
}
