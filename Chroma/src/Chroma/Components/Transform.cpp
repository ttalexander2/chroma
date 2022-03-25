#include "chromapch.h"
#include "Transform.h"

#include <glm/gtx/quaternion.hpp>


namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat({0,0,Rotation}));

		return Math::translate(glm::mat4(1.0f), { Position, 0 })
			* rotation * glm::scale(glm::mat4(1.0f), {Scale, 0});
	}

	void Transform::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Position";
		out << YAML::Value << Position;

		out << YAML::Key << "Rotation";
		out << YAML::Value << Rotation;

		out << YAML::Key << "Scale";
		out << YAML::Value << Scale;
	}
	void Transform::Deserialize(YAML::Node& node)
	{
		auto val = node["Position"];
		if (val)
		{
			Position = val.as<Math::vec2>();
		}

		val = node["Rotation"];
		if (val)
		{
			Rotation = val.as<float>();
		}

		val = node["Scale"];
		if (val)
		{
			Scale = val.as<Math::vec2>();
		}

	}

	void Transform::CreateReflectionModel()
	{
		entt::meta<Transform>()
			.data<&Transform::Position>("Position"_hs)
			.data<&Transform::Rotation>("Rotation"_hs)
			.data<&Transform::Scale>("Scale"_hs)
			.func<&Transform::GetTransform>("GetTransform"_hs)
			.type("Transform"_hs);
	}
}


