#pragma once

#include "Chroma/Scene/Component.h"

namespace Chroma
{
	class ScriptComponent : public Component
	{
	public:
		std::string Path;
		std::string ScriptName;
	};
}