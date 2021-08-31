#include "CodeEditor.h"
#include <fstream>
#include "Fonts/RobotoMono.cpp"
#include <Chroma/Core/Log.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <Chroma/Core/Input.h>

namespace Polychrome
{
	bool CodeEditor::Open;
	std::vector<CodeEditor::Instance> CodeEditor::Instances;
	ImFont* editor_font;
	TextEditor CodeEditor::Editor;
	std::filesystem::path CodeEditor::Current;
	std::filesystem::path prev;
	std::filesystem::path ToRemove;

	void CodeEditor::Init()
	{
		editor_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RobotoMono_compressed_data, RobotoMono_compressed_size, 16.0f);
		Editor.SetPalette(TextEditor::GetDarkPalette());
		Editor.SetShowWhitespaces(false);
	}

	void CodeEditor::Draw()
	{
		bool was_open = Open;
		bool editor_focused = false;
		if (Open)
		{
			if (ImGui::Begin("Code Editor", &Open, ImGuiWindowFlags_HorizontalScrollbar))
			{
				ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
				ImVec2 size = ImGui::GetContentRegionAvail();
				size.y = size.y - 35;
				if (ImGui::BeginTabBar("##text_editor_tab_bar", ImGuiTabBarFlags_Reorderable))
				{
					for (Instance& instance : Instances)
					{

						ImGuiTabItemFlags tab_flags = instance.Saved ? 0 : ImGuiTabItemFlags_UnsavedDocument;
						
						if (ImGui::BeginTabItem((instance.Path.filename().string() + "##text_editor_file").c_str(), &instance.Open, tab_flags))
						{
							editor_focused = true;
							
							if (instance.Path != Current)
							{
								Current = instance.Path;
								Editor.SetLanguageDefinition(instance.Definition);
								Editor.SetText(std::string(instance.Text));
								Editor.SetCursorPosition(instance.Coordinates);
							}

							ImGui::PushID(("##TextEditor_ID" + instance.Path.filename().string()).c_str());
							ImGui::PushFont(editor_font);
							Editor.Render(("##TextEditor" + instance.Path.filename().string()).c_str(), size);
							ImGui::PopFont();


							if (instance.Saved && Editor.IsTextChanged())
								instance.Saved = false;
							instance.Text = Editor.GetText();
							instance.Coordinates = Editor.GetCursorPosition();


							auto cpos = Editor.GetCursorPosition();
							ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, Editor.GetTotalLines(),
								Editor.IsOverwrite() ? "Ovr" : "Ins",
								Editor.CanUndo() ? "*" : " ",
								Editor.GetLanguageDefinition().mName.c_str(), instance.Path.string().c_str());

							ImGui::PopID();

							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}
			}

			ImGui::End();

			prev = Current;

			Instance* unsaved;
			for (Instance& i : Instances)
			{
				if (!i.Open && !i.Saved)
				{
					ImGui::OpenPopup("Close file?##editor_text_file");
					unsaved = &i;
					
				}
			}

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Close file?##editor_text_file", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("This file has unsaved changes. Would you like to save your work?");



				if (ImGui::Button("Save"))
				{
					std::ofstream stream(unsaved->Path);
					if (stream.is_open())
					{
						stream << unsaved->Text;
						unsaved->Saved = true;
					}
					else
					{
						unsaved->Open = true;
					}
					stream.close();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Discard"))
				{
					unsaved->Saved = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					unsaved->Open = true;
					ImGui::CloseCurrentPopup();
				}


				ImGui::EndPopup();
			}
			else
			{
				auto it = std::remove_if(Instances.begin(), Instances.end(), [&](Instance i) {
					return !i.Open;
					});
				Instances.erase(it, Instances.end());
			}


			if (Instances.empty())
				Open = false;


			for (Instance& i : Instances)
			{
				if (editor_focused && Current == i.Path && !i.Saved && (ImGui::IsKeyDown((int)CHROMA_KEY_LEFT_CONTROL) || ImGui::IsKeyDown((int)CHROMA_KEY_RIGHT_CONTROL)) && ImGui::IsKeyDown((int)CHROMA_KEY_S))
				{
					std::ofstream stream(i.Path);
					if (stream.is_open())
					{
						stream << i.Text;
						i.Saved = true;
					}
					stream.close();
				}
			}
		}
		
		if (was_open && !Open && Instances.size() > 0)
		{
			bool unsaved = false;
			for (Instance& i : Instances)
			{
				if (!i.Saved)
					unsaved = true;
			}
			if (unsaved)
				ImGui::OpenPopup("Close editor?##editor_text_file");
			else
				Instances.clear();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Close editor?##editor_text_file", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (Instances.size() == 1)
				ImGui::Text("This file has unsaved changes. Would you like to save your work?");
			else
				ImGui::Text("There are multiple files with unsaved changes. Would you like to save your work?");

			if (ImGui::Button("Save"))
			{
				bool all_saved = true;
				for (Instance& i : Instances)
				{
					if (!i.Saved)
					{
						std::ofstream stream(i.Path);
						if (stream.is_open())
						{
							stream << i.Text;
						}
						else
						{
							all_saved = false;
						}
						stream.close();
						i.Saved = true;
					}
				}
				if (!all_saved)
					Open = true;

				auto it = std::remove_if(Instances.begin(), Instances.end(), [&](Instance i) {
					return i.Saved;
					});
				Instances.erase(it, Instances.end());

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Discard"))
			{
				Instances.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				Open = true;
				ImGui::CloseCurrentPopup();
			}


			ImGui::EndPopup();
		}

	}

	CodeEditor::Instance::Instance(std::filesystem::path path) : Path(path)
	{
		std::ifstream t(Path, std::ifstream::in);
		if (t.good())
		{
			Text = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
		t.close();

	}
}