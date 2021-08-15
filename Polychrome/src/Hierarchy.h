#pragma once

#include <Chroma/Scene/Entity.h>

namespace Polychrome
{
	class Hierarchy
	{
	public:
		static void Draw();

	public:
		static bool Open;
		static Chroma::EntityID SelectedEntity;
	};
}


