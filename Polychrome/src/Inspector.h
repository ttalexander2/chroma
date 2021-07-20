#pragma once
#include <Chroma/Scene/Inspectable.h>
#include <Chroma/Scene/ECS.h>

namespace Polychrome
{
	class Inspector
	{
	public:
		static void Draw();
		
	public:
		static bool Open;


	private:


		static void DrawEntity();
	};

}

