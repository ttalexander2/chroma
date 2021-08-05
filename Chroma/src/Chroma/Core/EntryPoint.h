#pragma once

#include "Chroma/Profiler/Instrumentor.h"

#ifdef CHROMA_PLATFORM_WINDOWS


/// @brief Entry point for the application. This function should be overidden by the client.
/// @return Returns a new instance of the application.
extern Chroma::Application* Chroma::CreateApplication();


/// @brief Application entry point.
int main(int argc, char** argv)
{
	Chroma::Log::Init();
	CHROMA_PROFILE_BEGIN_SESSION("Startup", "ChromaProfile-Startup.json");
	auto app = Chroma::CreateApplication();
	CHROMA_PROFILE_END_SESSION();
	CHROMA_PROFILE_BEGIN_SESSION("Runtime", "ChromaProfile-Runtime.json");
	app->Run();
	CHROMA_PROFILE_END_SESSION();
	CHROMA_PROFILE_BEGIN_SESSION("Shutdown", "ChromaProfile-Shutdown.json");
	delete app;
	CHROMA_PROFILE_END_SESSION();

}

#endif