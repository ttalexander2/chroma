#pragma once

#ifdef CHROMA_PLATFORM_WINDOWS

extern Chroma::Application* Chroma::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Chroma::CreateApplication();
	app->Run();
	delete app;
}




#endif