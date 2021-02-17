#pragma once

#include "Core.h"

namespace Chroma
{
	class CHROMA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// Game entry point, defined in game project
	Application* CreateApplication();
}

