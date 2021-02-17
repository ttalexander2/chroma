#include "chromapch.h"
#include "Application.h"

#include "Chroma/Events/ApplicationEvent.h"
#include "Chroma/Log.h"

namespace Chroma
{
	
	Application::Application() 
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}
	Application::~Application() {}

	void Application::Run() 
	{
		while (true)
		{
			m_Window->OnUpdate();
		}
	}

}