#pragma once

#include "Widgets/TextEditor.h"
#include <filesystem>

namespace Polychrome
{
	class CodeEditor
	{
	public:

		static void Init();
		static void Draw();

		static bool Open;
		static TextEditor Editor;
		static std::filesystem::path Current;

		class Instance
		{
		public:
			Instance(std::filesystem::path path);
			std::filesystem::path Path;
			std::string Text;
			bool Saved = true;
			bool Open = true;
			TextEditor::LanguageDefinition Definition;
			TextEditor::Coordinates Coordinates;
		};

		static std::vector<Instance> Instances;
	};
}
