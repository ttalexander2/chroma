#pragma once
#include <glm/glm.hpp>
#include <Chroma.h>

namespace Polychrome
{
	class Viewport
	{
	public:
		static bool Open;
		static void Draw(Chroma::Ref<Chroma::Framebuffer> frame_buffer);
		
	};
}


