#pragma once

#include "Widgets/TextEditor.h"

namespace Polychrome
{
	class TextEdit
	{
	public:

		static void Init();
		static void Draw();

		static TextEditor Editor;
	};
}
