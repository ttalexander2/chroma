#pragma once
#include "Chroma/Math/Math.h"
#include "Chroma/Scene/EntityID.h"

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
		static bool DrawAllEntities;

		static bool DrawCameraGizmos;
		static bool DrawBoxCollider;
		static bool DrawSpriteBoundries;
		static bool DrawParticleEmtiters;

	private:
		static void DrawBoxColliderGizmos(Chroma::EntityID entity);
		static void DrawSpriteBoundryGizmos(Chroma::EntityID entity);

	};
}