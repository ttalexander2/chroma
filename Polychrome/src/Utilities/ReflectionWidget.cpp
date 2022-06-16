#include "ReflectionWidget.h"

#include "imgui.h"
#include <Chroma/Reflection/Reflection.h>

namespace Polychrome
{
	void DrawMetaAny(const std::string& label, entt::meta_any& val)
	{
		if (val.type().id() == entt::resolve<Math::vec2>().id())
		{
			Math::vec2* ptr = val.try_cast<Math::vec2>();
			float buff[2] = { ptr->x, ptr->y };
			if (ImGui::InputFloat2(label.c_str(), buff, "%.3f")) 
			{
				ptr->x = buff[0];
				ptr->y = buff[1];
			}
			//Math::vec2 result = { buff[0], buff[1] };
			//val.set(entt::resolve<Math::vec2>().id(), result);
		}
		else if (val.type().id() == entt::resolve<float>().id())
		{
			float* ptr = val.try_cast<float>();
			float buff = *ptr;
			if (ImGui::InputFloat(label.c_str(), &buff))
			{
				*ptr = buff;
			}
		}

	}

}

