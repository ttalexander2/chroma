#pragma once
#include "Chroma/Math/Math.h"
#include "Chroma/Scene/ECS.h"

namespace Polychrome
{
	class ComponentDebugGizmos
	{
	public:
		static void DrawGizmos();
		static void DrawIcons();
		static void DrawGrid();

		static Math::uvec2 GridSize;
		static Math::uvec2 SnapSize;
		static float GridOpacity;
		static bool DrawAllGizmos;
		static bool DrawCameraGizmos;
		static bool DrawBoxCollider;
		static bool DrawAllEntities;

	private:
		static void DrawBoxColliderGizmos(Chroma::EntityID entity);

	};
}