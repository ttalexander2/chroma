#include "chromapch.h"
#include "CircleCollider2D.h"
#include "imgui.h"

namespace Chroma
{
	void CircleCollider2D::DrawImGui()
	{
		if (ImGui::BeginTable("##circle_collider_2d_component_table_inspector", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
		{
			ImGui::TableNextColumn();

			ImGui::AlignTextToFramePadding(); ImGui::Text("Radius");
			ImGui::AlignTextToFramePadding(); ImGui::Text("Offset");

			ImGui::TableNextColumn();

			ImGui::InputFloat("##circle_collider_2d_bounds", &Radius);

			float off[2] = { Offset.x, Offset.y };
			if (ImGui::InputFloat2("##transform_rotation", off))
			{
				Offset = { off[0], off[1], };
			}

			ImGui::EndTable();
		}
	}
	void CircleCollider2D::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Radius";
		out << YAML::Value << Radius;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;
	}
	void CircleCollider2D::Deserialize(YAML::Node& node)
	{
		auto val = node["Radius"];
		if (val)
		{
			Radius = val.as<float>();
		}

		val = node["Offset"];
		if (val)
		{
			Offset = val.as<Math::vec2>();
		}

	}
}