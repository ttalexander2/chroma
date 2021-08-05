#pragma once

#include "Chroma/Core/Core.h"
#include "Chroma/Core/KeyCodes.h"
#include "Chroma/Core/MouseButtonCodes.h"

namespace Chroma
{
	/// @brief Input class for polling mouse and keyboard input.
	class CHROMA_API Input
	{
	public:
		/// @brief Check if a specific key is pressed.
		static bool IsKeyPressed(int keycode);
		/// @brief Check if a mouse button is pressed.
		static bool IsMouseButtonPressed(int button);
		/// @brief Get the current mouse position.
		static std::pair<float, float> GetMousePos();
		/// @brief Get the mouse X position.
		static float GetMouseX();
		/// @brief Get the mouse Y position.
		static float GetMouseY();
	};
}
