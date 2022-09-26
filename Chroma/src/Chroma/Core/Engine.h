#pragma once
#include "Core.h"
#include <Chroma/Scene/Scene.h>
#include <Chroma/IO/File.h>
#include <Chroma/IO/FileSystem.h>

namespace Chroma
{
	class Engine
	{
	public:
		static bool LoadScene(const std::string &sceneFile);
		static bool LoadScene(File &file);
		static bool SceneLoaded() { return CurrentScene != nullptr; }

		static Scene *CurrentScene;
	};
}
