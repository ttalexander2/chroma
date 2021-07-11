#include "chromapch.h"
#include "BoxCollider2D.h"
#include "imgui.h"

namespace Chroma
{
	void BoxCollider2D::DrawImGui()
	{
		DrawComponentValue("Bounds");
		float pos[2] = { Bounds.x, Bounds.y };
		if (ImGui::InputFloat2("##box_collider_2d_bounds", pos))
		{
			Bounds = { pos[0], pos[1] };
		}

		DrawComponentValue("Offset");
		float off[2] = { Offset.x, Offset.y };
		if (ImGui::InputFloat2("##box_collider_2d_offset", off))
		{
			Offset = { off[0], off[1], };
		}

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