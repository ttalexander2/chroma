#include "chromapch.h"
#include "FMODBank.h"

#include "AssetManager.h"
#include "Chroma/IO/File.h"
#include "Chroma/IO/FileSystem.h"
#include "Chroma/Audio/Audio.h"

namespace Chroma
{
	bool FMODBank::Load()
	{
		if (!FileSystem::Exists(GetPath()))
		{
			CHROMA_CORE_ERROR("Could not load bank! The following path does not exist: {}", GetPath());
			return false;
		}

		Audio::LoadBank(GetPath());
		return true;
	}

	bool FMODBank::Unload()
	{
		Audio::UnloadBank(GetPath());
		return true;
	}

	bool FMODBank::Reload()
	{
		if (AssetManager::Exists(GetID()))
			Audio::UnloadBank(GetPath());

		if (!FileSystem::Exists(GetPath()))
		{
			CHROMA_CORE_ERROR("Could not load bank! The following path does not exist: {}", GetPath());
			return false;
		}

		Audio::LoadBank(GetPath());
		return true;
	}
}
