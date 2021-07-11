#include "chromapch.h"
#include "ImGuiDebugMenu.h"
#include "imgui.h"

namespace Chroma
{
	static std::vector<std::pair<std::string, std::string>> vals;

	void ImGuiDebugMenu::ShowValue(std::string label, std::string value)
	{
		vals.push_back(std::pair<std::string, std::string>(label, value));
	}

	void ImGuiDebugMenu::ShowValue(std::string label, bool value)
	{
		ImGuiDebugMenu::ShowValue(label, std::to_string(value));
	}

	void ImGuiDebugMenu::ShowValue(std::string label, int value)
	{
		ImGuiDebugMenu::ShowValue(label, std::to_string(value));
	}

	void ImGuiDebugMenu::ShowValue(std::string label, float value)
	{
		ImGuiDebugMenu::ShowValue(label, std::to_string(value));
	}

	void ImGuiDebugMenu::ShowValue(std::string label, double value)
	{
		ImGuiDebugMenu::ShowValue(label, std::to_string(value));
	}

	void ImGuiDebugMenu::Draw()
	{
		if (ImGui::Begin("Debug"))
		{
			for (std::pair<std::string, std::string> pair : vals)
			{
				ImGui::Text((pair.first + ": ").c_str());
				ImGui::SameLine();
				ImGui::Text(pair.second.c_str());
			}
			ImGui::End();
		}

		vals.clear();
	}
}


