#include "chromapch.h"
#include "CircleCollider2D.h"
#include "imgui.h"
#include "Chroma/ImGui/Widgets/VecWithLabels.h"

namespace Chroma
{
	void CircleCollider2D::DrawImGui()
	{
		DrawComponentValue("Radius");
		ImGui::InputFloat("##circle_collider_2d_bounds", &Radius);

		DrawComponentValue("Offset");
		ImGui::Vec2IntWithLabels("##transform_rotation", Offset);


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