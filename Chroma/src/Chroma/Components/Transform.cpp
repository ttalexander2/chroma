#include "chromapch.h"
#include "Transform.h"

#include "Chroma/Reflection/Reflection.h"

#include <glm/gtx/quaternion.hpp>


namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat({0,0,Rotation}));

		return Math::translate(glm::mat4(1.0f), Math::vec3(Position.x, Position.y, 0.f))
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

		if (Parent != ENTITY_NULL)
		{
			out << YAML::Key << "Parent";
			out << YAML::Value << (uint32_t)Parent;
		}

		if (HasChildren())
		{
			out << YAML::Key << "Children";
			out << YAML::Value << YAML::Flow << YAML::BeginSeq;

			for (EntityID id : Children)
			{
				out << (uint32_t)id;
			}

			out << YAML::EndSeq;
		}
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

		val = node["Parent"];
		if (val)
		{
			Parent = (EntityID)val.as<uint32_t>();
		}

		val = node["Children"];
		if (val && val.IsSequence())
		{
			for (auto it = val.begin(); it != val.end(); it++)
			{
				const auto &item = *it;
				Children.push_back((EntityID)item.as<uint32_t>());
			}
		}

	}

	void Transform::CreateReflectionModel()
	{
		Reflection::RegisterComponent<Transform, Component>();
		Reflection::RegisterComponentProperty<Transform, &Transform::Position>("Position");
		Reflection::RegisterComponentProperty<Transform, &Transform::Rotation>("Rotation");
		Reflection::RegisterComponentProperty<Transform, &Transform::Scale>("Scale");
		Reflection::RegisterComponentFunction<Transform, &Transform::GetTransform>("GetTransform");
		Reflection::RegisterComponentProperty<Transform, &Transform::Parent>("Parent");
		Reflection::RegisterComponentProperty<Transform, &Transform::Children>("Children");
		Reflection::RegisterComponentFunction<Transform, &Transform::HasChild>("HasChild");
		Reflection::RegisterComponentFunction<Transform, &Transform::HasChildren>("HasChildren");
	}
}


