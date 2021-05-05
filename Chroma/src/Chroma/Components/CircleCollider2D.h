#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec2.h"

namespace Chroma
{
	struct CircleCollider2D : Component
	{
		float Radius = 1.0f;
		Math::vec2 Offset{ 0.0f, 0.0f };

		const std::string Name() const override
		{
			return "Circle Collider 2D";
		}

		void DrawImGui() override
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

		void Serialize(YAML::Emitter& out) override
		{
			out << YAML::Key << "Radius";
			out << YAML::Value << Radius;

			out << YAML::Key << "Offset";
			out << YAML::Value << Offset;
		}

		void Deserialize(YAML::Node& node) override
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
	};
}

