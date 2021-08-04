#include "chromapch.h"
#include "BoxCollider2D.h"
#include "imgui.h"
#include "Chroma/ImGui/Widgets/VecWithLabels.h"

namespace Chroma
{
	void BoxCollider2D::DrawImGui()
	{
		DrawComponentValue("Bounds");
		ImGui::Vec2IntWithLabels("##box_collider_2d_bound", Bounds);

		DrawComponentValue("Offset");
		ImGui::Vec2IntWithLabels("##box_collider_2d_offset", Offset);
	}
	void BoxCollider2D::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Bounds";
		out << YAML::Value << Bounds;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;
	}
	void BoxCollider2D::Deserialize(YAML::Node& node)
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
}