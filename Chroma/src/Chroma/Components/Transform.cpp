#include "chromapch.h"
#include "Transform.h"

#include "Chroma/Reflection/Reflection.h"

#include <glm/gtx/quaternion.hpp>


namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = toMat4(glm::quat({ 0, 0, Rotation }));

		return translate(glm::mat4(1.0f), Math::vec3(Position.x, Position.y, 0.f))
				* rotation * scale(glm::mat4(1.0f), { Scale, 0 });
	}

	Reflection::TypeFactory<Transform> Transform::RegisterType()
	{
		return Reflection::Register<Transform>("Transform")
		       .Base<Component>()
		       .Data<&Transform::Position>("Position")
		       .Data<&Transform::Rotation>("Rotation")
		       .Data<&Transform::Scale>("Scale")
		       .Data<&Transform::Parent>("Parent",
				       [](Reflection::Any *val)
				       {
					       return val->Cast<Transform>().Parent != ENTITY_NULL;
				       })
		       .Data<&Transform::Children>("Children")
		       .Func<&Transform::HasChild>("HasChild")
		       .Func<&Transform::HasChildren>("HasChildren")
		       .Func<&Transform::GetTransform>("GetTransform");
	}
}
