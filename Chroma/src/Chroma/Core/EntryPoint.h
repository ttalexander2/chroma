#pragma once

#include "Chroma.h"
#include "Chroma/Profiler/Instrumentor.h"

#ifdef CHROMA_PLATFORM_WINDOWS


/// @brief Entry point for the application. This function should be overidden by the client.
/// @return Returns a new instance of the application.
extern Chroma::Application* Chroma::CreateApplication();

void InitFilesystem();
void DeinitFilesystem();


#endif