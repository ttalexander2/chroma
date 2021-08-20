#include "TextEdit.h"
#include <fstream>
#include "Fonts/RobotoMono.cpp"
#include <Chroma/Core/Log.h>

namespace Polychrome
{
	bool TextEdit::Open;
	std::vector<TextEdit::Instance> TextEdit::Instances;
	ImFont* editor_font;
	TextEditor TextEdit::Editor;
	std::filesystem::path TextEdit::Current;
	std::filesystem::path ToRemove;

	void TextEdit::Init()
	{
		editor_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RobotoMono_compressed_data, RobotoMono_compressed_size, 16.0f);
		Editor.SetPalette(TextEditor::GetDarkPalette());
		Editor.SetShowWhitespaces(false);
	}

	void TextEdit::Draw()
	{
		if (Open)
		{
			if (ImGui::Begin("Text Editor", &Open, ImGuiWindowFlags_HorizontalScrollbar))
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

							if (Current != instance.Path)
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


			for (Instance& i : Instances)
			{
				if (!i.Open && !i.Saved)
				{

				}
			}
			auto it = std::remove_if(Instances.begin(), Instances.end(), [&](Instance i) {
				return !i.Open;
			});
			Instances.erase(it, Instances.end());
			ToRemove.clear();

			if (Instances.empty())
				Open = false;
		}

	}

	TextEdit::Instance::Instance(std::filesystem::path path) : Path(path)
	{
		std::ifstream t(Path, std::ifstream::in);
		if (t.good())
		{
			Text = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
		t.close();

	}
}