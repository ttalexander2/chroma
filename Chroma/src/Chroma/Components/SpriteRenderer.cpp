#include "chromapch.h"
#include "SpriteRenderer.h"
#include "Chroma/ImGui/ImGuiComponents/Vec3WithLabels.h"

namespace Chroma
{
	void SpriteRenderer::DrawImGui()
	{
		auto style = ImGui::GetStyle();
		if (ImGui::BeginTable("##sprite_renderer_component_table_inspector", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
		{
			ImGui::TableNextColumn();

			ImGui::AlignTextToFramePadding(); ImGui::Text("Color  ");

			ImGui::TableNextColumn();

			//ImGui::PushItemWidth(0);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - 6.0f);

			float col[4]{ Color.r, Color.b, Color.g, Color.a };
			if (ImGui::ColorEdit4(("##sprite_renderer_color" + std::to_string(this->GetUniqueID())).c_str(), col))
			{
				Color = { col[0], col[1], col[2], col[3] };
			}
			ImGui::PopStyleVar();

			//ImGui::PopItemWidth();
			ImGui::EndTable();

		}
	}
	void SpriteRenderer::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Color";
		out << YAML::Value << Color;
	}
	void SpriteRenderer::Deserialize(YAML::Node& node)
	{
		auto val = node["Color"];
		if (val)
		{
			Color = val.as<Math::vec4>();
		}

	}
}


