#pragma once
#include <imgui.h>
#include "Chroma/Math/Vec3.h"

namespace Chroma
{

	static bool Vec3IntWithLabels(const char* hash, Math::vec3& val)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 3.0f - (6.0f * 6)) / 3;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });
		ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str());
		ImGui::PopStyleColor();
		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		int pos = val.x;
		if (ImGui::DragInt(("##vec3_x" + std::string(hash)).c_str(), &pos))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str());
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragInt(("##vec3_y" + std::string(hash)).c_str(), &pos))
		{
			val.y = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.1f, 0.8f, 0.8f });
		ImGui::Button(("Z##vec3_z_label" + std::string(hash)).c_str());
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.z;
		if (ImGui::DragInt(("##vec3_z" + std::string(hash)).c_str(), &pos))
		{
			val.z = pos;
		}

		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 6.0f);
		ImGui::NewLine();
	}

	static bool Vec3FloatWithLabels(const char* hash, Math::vec3& val)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 3.0f - (6.0f * 6)) / 3;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });

		ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str());

		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);


		float pos = val.x;
		if (ImGui::DragFloat(("##vec3_x" + std::string(hash)).c_str(), &pos))
		{
			val.x = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
		ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str());
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);

		pos = val.y;
		if (ImGui::DragFloat(("##vec3_y" + std::string(hash)).c_str(), &pos))
		{
			val.y = pos;
		}

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.1f, 0.8f, 0.8f });
		ImGui::Button(("Z##vec3_z_label" + std::string(hash)).c_str());
		ImGui::PopStyleColor();

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
	}
}