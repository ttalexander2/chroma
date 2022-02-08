#include "ErrorWindow.h"
#include "Build.h"
#include "Project.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <algorithm>
#include "Fonts/IconsForkAwesome.h"
#include <filesystem>
#include <Chroma/Assets/AssetManager.h>


namespace Polychrome
{
	bool ErrorWindow::Open = true;

	enum ErrorColumn
	{
		ErrorColumn_Code,
		ErrorColumn_Message,
		ErrorColumn_File,
		ErrorColumn_Location,
		ErrorColumn_Description
	};

	void ErrorWindow::Draw()
	{
		if (!Open)
			return;

		if (ImGui::Begin("Errors##ERROR_WINDOW", &Open))
		{
			static bool show_errors = true;
			static bool show_warnings = true;
			static bool show_messages = true;

			ImGui::Dummy({ 1,1 });
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 220.f);

			bool pop = false;
			if (!show_errors)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				pop = true;
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.15f, 1.f));
			if (ImGui::Button(ICON_FK_EXCLAMATION_CIRCLE " Errors")) show_errors = !show_errors;
			ImGui::PopStyleColor();
			if (pop)
				ImGui::PopStyleColor();

			ImGui::SameLine();
			pop = false;

			if (!show_warnings)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				pop = true;
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.1f, 1.f));
			if (ImGui::Button(ICON_FK_EXCLAMATION_TRIANGLE " Warnings")) show_warnings = !show_warnings;
			ImGui::PopStyleColor();
			if (pop)
				ImGui::PopStyleColor();

			ImGui::SameLine();
			pop = false;

			if (!show_messages)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				pop = true;
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.7f, 1.5f, 1.f));
				if (ImGui::Button(ICON_FK_INFO_CIRCLE " Messages")) show_messages = !show_messages;
			ImGui::PopStyleColor();
			if (pop)
				ImGui::PopStyleColor();


			static ImGuiTableFlags flags =
				ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

			if (ImGui::BeginTable("##error_table", 5, flags))
			{
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHide, 0.f, ErrorColumn_Code);
				ImGui::TableSetupColumn("Code", 0, 0.f, ErrorColumn_Code);
				ImGui::TableSetupColumn("Message", 0, 0.f, ErrorColumn_Message);
				ImGui::TableSetupColumn("File", 0, 0.f, ErrorColumn_File);
				ImGui::TableSetupColumn("Location", 0, 0.f, ErrorColumn_Location);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();

				if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
				{
					if (sort_specs->SpecsDirty && Build::BuildMessages.size() > 1)
					{
						std::sort(Build::BuildMessages.begin(), Build::BuildMessages.end(), [&](const Build::BuildMessage& a, const Build::BuildMessage& b) {
							for (int n = 0; n < sort_specs->SpecsCount; n++)
							{
								const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[n];
								int delta = 0;
								switch (sort_spec->ColumnUserID)
								{
								case ErrorColumn_Code:			delta = (strcmp(a.error.c_str(), b.error.c_str()));			break;
								case ErrorColumn_Message:		delta = (strcmp(a.message.c_str(), b.message.c_str()));		break;
								case ErrorColumn_File:			delta = (strcmp(a.file.c_str(), b.file.c_str()));			break;
								case ErrorColumn_Location:		delta = a.line - b.line;									break;
								default:						delta = 0;													break;
								}
								if (delta > 0)
									return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? true: false;
								if (delta < 0)
									return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? false : true;
							}
							return strcmp(a.error.c_str(), b.error.c_str()) > 0;
						});

						sort_specs->SpecsDirty = false;
					}
				}

				static int selected = -1;
				int i = 0;
				for (Build::BuildMessage& error : Build::BuildMessages)
				{
					if (error.severity == Build::BuildMessage::Severity::Error && !show_errors)
						continue;
					else if (error.severity == Build::BuildMessage::Severity::Warning && !show_warnings)
						continue;
					else if (error.severity == Build::BuildMessage::Severity::Message && !show_messages)
						continue;

					ImGui::PushID(&error);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					bool is_selected = (selected == i);

					ImGui::Selectable("##select", &is_selected, ImGuiSelectableFlags_SpanAllColumns);
					if (is_selected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
					{
						system(("code " + std::filesystem::path(Project::AssetDirectory).parent_path().string() + " -g " + error.file + ":" + std::to_string(error.line) + ":" + std::to_string(error.offset)).c_str());
					}

					ImGui::SameLine();



					if (error.severity == Build::BuildMessage::Severity::Error)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.15f, 1.f));
						ImGui::Text(ICON_FK_EXCLAMATION_CIRCLE);
						ImGui::PopStyleColor();
					}
					else if (error.severity == Build::BuildMessage::Severity::Warning)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.1f, 1.f));
						ImGui::Text(ICON_FK_EXCLAMATION_TRIANGLE);
						ImGui::PopStyleColor();
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.3f, 0.8f, 1.f));
						ImGui::Text(ICON_FK_INFO_CIRCLE);
						ImGui::PopStyleColor();
					}

					ImGui::TableNextColumn();
					ImGui::Text(error.error.c_str());
					ImGui::TableNextColumn();
					ImGui::Text(error.message.c_str());
					ImGui::TableNextColumn();
					ImGui::Text(std::filesystem::relative(error.file, Project::AssetDirectory).string().c_str());
					ImGui::TableNextColumn();
					ImGui::Text(("(" + std::to_string(error.line) + "," + std::to_string(error.offset) + ")").c_str());
					ImGui::PopID();

					if (is_selected)
					{
						selected = i;
					}

					i++;
				}
				


				ImGui::EndTable();
			}

		}
		ImGui::End();
	}
}