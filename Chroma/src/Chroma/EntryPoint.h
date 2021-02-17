#pragma once

#ifdef CHROMA_PLATFORM_WINDOWS

extern Chroma::Application* Chroma::CreateApplication();

int main(int argc, char** argv)
{
	Chroma::Log::Init();
	CHROMA_CORE_WARN("Initialized Log!");
	CHROMA_ERROR("oof");

	auto app = Chroma::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif