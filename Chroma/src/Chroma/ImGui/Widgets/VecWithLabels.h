#pragma once
#include <imgui.h>
#include "Chroma/Math/Vec3.h"
#include "Chroma/Math/Vec2.h"
#include <string>

namespace ImGui
{


	/// @brief Displays 3 component vectors with component labels (x,y,z) and reset buttons.
	/// Vector float components will be clamped to integers.
	/// @param hash Label to display.
	/// @param val Value reference to display.
	/// @param reset_to_zero Whether clicking the reset should set to zero. False will reset to one.
	static bool Vec3IntWithLabels(const char* hash, Math::vec3& val, bool reset_to_zero = true)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 3.0f - (6.0f * 6)) / 3;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });
		if (ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.x = 0;
			}
			else
			{
				val.x = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		float pos = val.x;
		if (ImGui::DragFloat(("##vec3_x" + std::string(hash)).c_str(), &pos, 1.f))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		if (ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.y = 0;
			}
			else
			{
				val.y = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragFloat(("##vec3_y" + std::string(hash)).c_str(), &pos, 1.f))
		{
			val.y = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.1f, 0.8f, 0.8f });
		if (ImGui::Button(("Z##vec3_z_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.z = 0;
			}
			else
			{
				val.z = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.z;
		if (ImGui::DragFloat(("##vec3_z" + std::string(hash)).c_str(), &pos, 1.0f))
		{
			val.z = pos;
		}

		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 6.0f);
		ImGui::NewLine();
		return true;
	}

	/// @brief Displays 3 component vectors with component labels (x,y,z) and reset buttons.
	/// @param hash Label to display.
	/// @param val Value reference to display.
	/// @param reset_to_zero Whether clicking the reset should set to zero. False will reset to one.
	static bool Vec3FloatWithLabels(const char* hash, Math::vec3& val, bool reset_to_zero = true)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 3.0f - (6.0f * 6)) / 3;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });

		if (ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.x = 0;
			}
			else
			{
				val.x = 1;
			}
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);


		float pos = val.x;
		if (ImGui::DragFloat(("##vec3_x" + std::string(hash)).c_str(), &pos))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		if (ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.y = 0;
			}
			else
			{
				val.y = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragFloat(("##vec3_y" + std::string(hash)).c_str(), &pos))
		{
			val.y = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.1f, 0.8f, 0.8f });
		if (ImGui::Button(("Z##vec3_z_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.z = 0;
			}
			else
			{
				val.z = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.z;
		if (ImGui::DragFloat(("##vec3_z" + std::string(hash)).c_str(), &pos))
		{
			val.z = pos;
		}

		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 6.0f);
		ImGui::NewLine();
		return true;
	}

	/// @brief Displays 2 component vectors with component labels (x,y) and reset buttons.
	/// Vector float components will be clamped to integers.
	/// @param hash Label to display.
	/// @param val Value reference to display.
	/// @param reset_to_zero Whether clicking the reset should set to zero. False will reset to one.
	static bool Vec2IntWithLabels(const char* hash, Math::vec2& val, bool reset_to_zero = true)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 2.0f - (4.0f * 6)) / 2;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });
		if (ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.x = 0;
			}
			else
			{
				val.x = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		int pos = (int)val.x;
		if (ImGui::DragInt(("##vec3_x" + std::string(hash)).c_str(), &pos))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		if (ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.y = 0;
			}
			else
			{
				val.y = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragInt(("##vec3_y" + std::string(hash)).c_str(), &pos))
		{
			val.y = pos;
		}

		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 6.0f);
		ImGui::NewLine();
		return true;
	}

	/// @brief Displays 2 component vectors with component labels (x,y) and reset buttons.
	/// Vector float components will be clamped to integers.
	/// @param hash Label to display.
	/// @param val Value reference to display.
	/// @param reset_to_zero Whether clicking the reset should set to zero. False will reset to one.
	static bool UVec2IntNoReset(const char* hash, Math::uvec2& val)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 2.0f - (4.0f * 6)) / 2;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });
		ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str());

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		int pos = (int)val.x;
		if (ImGui::DragInt(("##vec3_x" + std::string(hash)).c_str(), &pos, 1.0f, 1))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str());

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragInt(("##vec3_y" + std::string(hash)).c_str(), &pos, 1.0f, 1))
		{
			val.y = pos;
		}

		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 6.0f);
		ImGui::NewLine();
		return true;
	}

	/// @brief Displays 2 component vectors with component labels (x,y) and reset buttons.
	/// @param hash Label to display.
	/// @param val Value reference to display.
	/// @param reset_to_zero Whether clicking the reset should set to zero. False will reset to one.
	static bool Vec2FloatWithLabels(const char* hash, Math::vec2& val, bool reset_to_zero = true)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 2.0f - (4.0f * 6)) / 2;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });

		if (ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.x = 0;
			}
			else
			{
				val.x = 1;
			}
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);


		float pos = val.x;
		if (ImGui::DragFloat(("##vec2_x" + std::string(hash)).c_str(), &pos))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		if (ImGui::Button(("Y##vec2_y_label" + std::string(hash)).c_str()))
		{
			if (reset_to_zero)
			{
				val.y = 0;
			}
			else
			{
				val.y = 1;
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click to reset value.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragFloat(("##vec3_y" + std::string(hash)).c_str(), &pos))
		{
			val.y = pos;
		}

		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 6.0f);
		ImGui::NewLine();
		return true;
	}
}