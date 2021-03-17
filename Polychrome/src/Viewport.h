#pragma once
#include <glm/glm.hpp>
#include <Chroma.h>

namespace Polychrome
{
	class Viewport
	{
	public:
		static void Render(Chroma::Ref<Chroma::Framebuffer> frame_buffer);
		
	};
}


