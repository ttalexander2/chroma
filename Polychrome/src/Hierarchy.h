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

		static void DrawEntity(Chroma::Scene* scene, Chroma::EntityID id, Chroma::EntityID next, bool root = true);
		static void DragDropSeparator(Chroma::Scene* scene, Chroma::EntityID curr, Chroma::EntityID next, float size = 7);
		static bool	IsRelative(Chroma::Scene* scene, Chroma::EntityID parent, Chroma::EntityID child);
	};
}


