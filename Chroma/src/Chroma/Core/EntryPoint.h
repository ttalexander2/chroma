#pragma once

#include "Chroma/Profiler/Instrumentor.h"

#ifdef CHROMA_PLATFORM_WINDOWS



extern Chroma::Application* Chroma::CreateApplication();

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