#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec4.h";
#include <imgui.h>

namespace Chroma
{
	struct SpriteRenderer : Component
	{
		Math::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		unsigned int Layer = 0;

		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(const Math::vec4 color)
			: Color(color)
		{
		}

		const std::string Name() const override
		{
			return "Sprite Renderer";
		}

		void DrawImGui() override
		{
			if (ImGui::BeginTable("##sprite_renderer_component_table_inspector", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
			{
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding(); ImGui::Text("Color   ");

				ImGui::TableNextColumn();

				float c[4] = { Color.x, Color.y, Color.z, Color.w };
				if (ImGui::ColorEdit4("##circle_collider_2d_bounds", c))
				{
					Color = { c[0], c[1], c[2], c[3] };
				}

				ImGui::EndTable();
			}
		}

		void Serialize(YAML::Emitter& out) override
		{
			out << YAML::Key << "Color";
			out << YAML::Value << Color;
		}

		void Deserialize(YAML::Node& node) override
		{
			auto val = node["Color"];
			if (val)
			{
				Color = val.as<Math::vec4>();
			}

		}
	};
}
