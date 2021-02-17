#include "chromapch.h"
#include "Application.h"

#include "Chroma/Events/ApplicationEvent.h"
#include "Chroma/Log.h"

namespace Chroma
{
	
	Application::Application() {}
	Application::~Application() {}

	void Application::Run() 
	{
		WindowResizeEvent e(1920, 1080);
		CHROMA_TRACE(e);
		while (true);
	}

}