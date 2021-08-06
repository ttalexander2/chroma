#include "chromapch.h"
#include "ImGuiHelper.h"
#include "imgui.h"

namespace ImGui
{
	/// @brief Counter for ImGui IDs
	static int id_counter = 0;


	void PushNewID()
	{
		ImGui::PushID(id_counter);
		id_counter++;
	}


	void ResetCounter()
	{
		id_counter = 0;
	}
}