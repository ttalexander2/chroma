#pragma once

#include <Chroma/Scene/EntityRef.h>

namespace Polychrome
{
	class Hierarchy
	{
	public:
		static void Draw();

	public:
		static bool Open;
		static Chroma::EntityRef SelectedEntity;
	};
}


