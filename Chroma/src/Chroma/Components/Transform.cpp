#include "chromapch.h"
#include "Transform.h"

namespace Chroma
{
	Math::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Position.y, { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

		return Math::translate(glm::mat4(1.0f), Position)
			* rotation * glm::scale(glm::mat4(1.0f), Scale);
	}
	void Transform::DrawImGui()
	{

		auto style = ImGui::GetStyle();

		if (ImGui::BeginTable("##transform_component_table_inspector", 2, ImGuiTableFlags_None | ImGuiTableFlags_SizingFixedFit))
		{
			ImGui::TableNextColumn();

			ImGui::AlignTextToFramePadding(); ImGui::Text("Position");
			ImGui::AlignTextToFramePadding(); ImGui::Text("Rotation");
			ImGui::AlignTextToFramePadding(); ImGui::Text("Scale");

			ImGui::TableNextColumn();

			//ImGui::PushItemWidth(0);

			Chroma::Vec3IntWithLabels(("##transform_position" + std::to_string(this->GetUniqueID())).c_str(), Position);
			Chroma::Vec3FloatWithLabels(("##transform_rotation" + std::to_string(this->GetUniqueID())).c_str(), Rotation);
			Chroma::Vec3IntWithLabels(("##transform_scale" + std::to_string(this->GetUniqueID())).c_str(), Scale);

			//ImGui::PopItemWidth();
			ImGui::EndTable();
		}
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


