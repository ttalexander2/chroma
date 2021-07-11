#pragma once
#include <Chroma/Scene/Inspectable.h>
namespace Polychrome
{
	class Inspector
	{
	public:
		static void Draw();
		static void SetInspectable(Chroma::Inspectable& inspect);
		
	public:
		static bool Open;


	private:
		static Chroma::Inspectable* Inspecting;
		static Chroma::Inspectable* Previous;
		static void DrawEntity();
	};

}

