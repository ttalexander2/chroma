#include "chromapch.h"
#include "FMODBank.h"

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

		Chroma::Audio::LoadBank(GetPath());
		return true;
	}

	bool FMODBank::Unload()
	{
		return true;
	}

	bool FMODBank::Reload()
	{
	    Chroma::Audio::LoadBank(GetPath());
		return true;
	}
}


