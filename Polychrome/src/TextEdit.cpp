#include "TextEdit.h"
#include <fstream>
#include "Fonts/RobotoMono.cpp"

namespace Polychrome
{
	TextEditor TextEdit::Editor;

	std::string fileToEdit = "assets/scripts/Test.lua";
	ImFont* editor_font;

	void TextEdit::Init()
	{
		auto& lang = TextEditor::LanguageDefinition::Lua();
		Editor.SetLanguageDefinition(lang);
		TextEditor::ErrorMarkers markers;
		markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
		markers.insert(std::make_pair<int, std::string>(12, "Another error here!"));
		Editor.SetErrorMarkers(markers);

		Editor.SetPalette(TextEditor::GetDarkPalette());
		Editor.SetShowWhitespaces(false);



		std::ifstream t(fileToEdit.c_str(), std::ifstream::in);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			Editor.SetText(str);
		}


		editor_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RobotoMono_compressed_data, RobotoMono_compressed_size, 16.0f);


	}

	void TextEdit::Draw()
	{

		auto cpos = Editor.GetCursorPosition();
		if (ImGui::Begin("Text Editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

			ImVec2 size = ImGui::GetContentRegionAvail();
			size.y = size.y - 15;


			ImGui::PushFont(editor_font);
			Editor.Render("TextEditor", size);
			ImGui::PopFont();

			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, Editor.GetTotalLines(),
				Editor.IsOverwrite() ? "Ovr" : "Ins",
				Editor.CanUndo() ? "*" : " ",
				Editor.GetLanguageDefinition().mName.c_str(), fileToEdit.c_str());
		}


		ImGui::End();
	}
}