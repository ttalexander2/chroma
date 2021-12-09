#include "chromapch.h"
#include "EntryPoint.h"
#include <physfs.h>

void InitFilesystem()
{
	PHYSFS_init(NULL);
}

void DeinitFilesystem()
{
	PHYSFS_deinit();
}

/// @brief Application entry point.
int main(int argc, char** argv)
{
	Chroma::Log::Init();
	InitFilesystem();
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