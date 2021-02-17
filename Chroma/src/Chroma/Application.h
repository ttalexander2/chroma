#pragma once

#include "chromapch.h"

#include "Core.h"
#include "Chroma/Window.h"

namespace Chroma
{
	class CHROMA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		std::unique_ptr<Window> m_Window;
		bool running = true;
	};

	// Game entry point, defined in game project
	Application* CreateApplication();
}

