#pragma once

#include "Chroma/Core/Core.h"
#include "Chroma/Core/KeyCodes.h"
#include "Chroma/Core/MouseButtonCodes.h"

namespace Chroma
{
	class CHROMA_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}
