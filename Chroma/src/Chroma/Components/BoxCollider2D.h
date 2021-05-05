#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/vec2.h"

namespace Chroma
{
	struct BoxCollider2D : Component
	{
		Math::vec2 Bounds{ 1.0f, 1.0f };
		Math::vec2 Offset{ 0.0f, 0.0f };

		BoxCollider2D() = default;
		BoxCollider2D(const BoxCollider2D&) = default;
		BoxCollider2D(const Math::vec2& bounds, const Math::vec2& offset)
			: Bounds(bounds), Offset(offset)
		{
		}

		const std::string Name() const override
		{
			return "Box Collider 2D";
		}

		void DrawImGui() override
		{
			if (ImGui::BeginTable("##box_collider_2d_component_table_inspector", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
			{
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding(); ImGui::Text("Bounds  ");
				ImGui::AlignTextToFramePadding(); ImGui::Text("Offset");

				ImGui::TableNextColumn();

				float pos[2] = { Bounds.x, Bounds.y };
				if (ImGui::InputFloat2("##box_collider_2d_bounds", pos))
				{
					Bounds = { pos[0], pos[1] };
				}
				float off[2] = { Offset.x, Offset.y };
				if (ImGui::InputFloat2("##transform_rotation", off))
				{
					Offset = { off[0], off[1], };
				}
				ImGui::EndTable();
			}
		}

		void Serialize(YAML::Emitter& out) override
		{
			out << YAML::Key << "Bounds";
			out << YAML::Value << Bounds;

			out << YAML::Key << "Offset";
			out << YAML::Value << Offset;
		}

		void Deserialize(YAML::Node& node) override
		{
			
			auto val = node["Bounds"];
			if (val)
			{
				Bounds = val.as<Math::vec2>();
			}

			val = node["Offset"];
			if (val)
			{
				Offset = val.as<Math::vec2>();
			}

		}
	};
}

