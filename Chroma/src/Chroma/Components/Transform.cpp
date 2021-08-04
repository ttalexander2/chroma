#include "chromapch.h"
#include "Transform.h"
#include "imgui_internal.h"
#include "Chroma/ImGui/Widgets/VecWithLabels.h"

#include <glm/gtx/quaternion.hpp>


namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return Math::translate(glm::mat4(1.0f), Position)
			* rotation * glm::scale(glm::mat4(1.0f), Scale);
	}
	void Transform::DrawImGui()
	{

		DrawComponentValue("Position");
		ImGui::Vec3IntWithLabels(("##transform_position" + std::to_string(this->GetUniqueID())).c_str(), Position);

		DrawComponentValue("Rotation");
		glm::vec3 rot = glm::degrees(Rotation);
		ImGui::Vec3FloatWithLabels(("##transform_rotation" + std::to_string(this->GetUniqueID())).c_str(), rot);
		Rotation = glm::radians(rot);

		DrawComponentValue("Scale");
		ImGui::Vec3FloatWithLabels(("##transform_scale" + std::to_string(this->GetUniqueID())).c_str(), Scale, false);


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
			Position = val.as<Math::vec3>();
		}

		val = node["Rotation"];
		if (val)
		{
			Rotation = val.as<Math::vec3>();
		}

		val = node["Scale"];
		if (val)
		{
			Scale = val.as<Math::vec3>();
		}

	}
}


