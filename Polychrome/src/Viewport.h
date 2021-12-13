#pragma once
#include <glm/glm.hpp>
#include <Chroma.h>
#include "imgui.h"

namespace Polychrome
{
	class Viewport
	{
	public:
		static bool Open;
		static void Draw(Chroma::Time time, Chroma::Ref<Chroma::Framebuffer> frame_buffer, Chroma::Ref<Chroma::Framebuffer> guizmo_framebuffer);
		static const Math::vec2 ViewportPositionToWorld(const Math::vec2& pos);
		static const Math::vec2 WorldToViewportPosition(const Math::vec2& pos);

		static bool IsViewportFocused();
		static bool IsViewportHovered();
		static ImVec2 GetViewportMousePos();
		static bool ShouldDrawGrid;
		static bool SnapToGrid;
	};
}


