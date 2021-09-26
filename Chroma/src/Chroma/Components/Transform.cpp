#include "chromapch.h"
#include "Transform.h"
#include "imgui_internal.h"
#include "Chroma/ImGui/Widgets/VecWithLabels.h"

#include <glm/gtx/quaternion.hpp>


namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat({0,0,Rotation}));

		return Math::translate(glm::mat4(1.0f), { Position, 0 })
			* rotation * glm::scale(glm::mat4(1.0f), {Scale, 0});
	}
	void Transform::DrawImGui()
	{

		DrawComponentValue("Position");
		ImGui::Vec2IntWithLabels(("##transform_position" + std::to_string(this->GetUniqueID())).c_str(), Position);

		DrawComponentValue("Rotation");
		float rot = glm::degrees(Rotation);
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth(2) - 2);
		ImGui::SliderFloat(("##transform_rotation" + std::to_string(this->GetUniqueID())).c_str(), &rot, 0.f, 360.f);
		Rotation = glm::radians(rot);

		DrawComponentValue("Scale");
		ImGui::Vec2FloatWithLabels(("##transform_scale" + std::to_string(this->GetUniqueID())).c_str(), Scale, false);


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
}


