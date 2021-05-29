#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Mat4x4.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Chroma/ImGui/ImGuiComponents/Vec3WithLabels.h"

#include "imgui.h"


namespace Chroma
{
	struct Transform : public Component
	{
		Math::vec3 Position = { 0.0f, 0.0f, 0.0f };
		Math::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		Math::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const Math::mat4& transform)
		{
		}

		Math::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Position.y, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

			return Math::translate(glm::mat4(1.0f), Position)
				* rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		const std::string Name() const override
		{
			return "Transform";
		}

		const bool AllowMultiple() const override { return false; };

		void DrawImGui() override
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

		void Serialize(YAML::Emitter& out) override
		{
			out << YAML::Key << "Position";
			out << YAML::Value << Position;

			out << YAML::Key << "Rotation";
			out << YAML::Value << Rotation;

			out << YAML::Key << "Scale";
			out << YAML::Value << Scale;
		}


		void Deserialize(YAML::Node& node) override
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

	private:
		const bool IsTransform() const override
		{
			return true;
		}
		

	};
}


