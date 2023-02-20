#include "chromapch.h"
#include "Transform.h"

#include "Chroma/Reflection/Reflection.h"

#include <glm/gtx/quaternion.hpp>


namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		const glm::mat4 rotation = toMat4(glm::quat({ 0, 0, Rotation }));

		return translate(glm::mat4(1.0f), Math::vec3(Position.x, Position.y, 0.f))
				* rotation * scale(glm::mat4(1.0f), { Scale, 0 });
	}

	Reflection::type_factory<Transform> Transform::register_type()
	{
		return Reflection::RegisterComponent<Transform>("Transform")
		       .base<Component>()
		       .data<&Transform::Position>("Position")
		       .data<&Transform::Rotation>("Rotation")
		       .data<&Transform::Scale>("Scale")
		       .data<&Transform::Parent>("Parent")
		       .data<&Transform::Children>("Children")
		       .function<&Transform::HasChild>("HasChild")
		       .function<&Transform::HasChildren>("HasChildren")
		       .function<&Transform::GetTransform>("GetTransform");
	}
}
