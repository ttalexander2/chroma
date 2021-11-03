#pragma once

#include <Chroma/Assets/AssetManager.h>
#include "Chroma/Utilities/GUID.h"

namespace Chroma
{
	/// @brief Generic game Asset
	///
	/// Interface for game assets. Each asset controlls its name, id, and path.
	/// Any implementing class is required to provide load/unload/reload functionality.
	struct Asset
	{
		friend class AssetManager;

		const std::string GetPath() const { return Path; }
		const GUID GetID() const { return ID; }
		virtual bool Load() = 0;
		virtual bool Unload() = 0;
		virtual bool Reload() = 0;

	protected:
		std::string Path;
		GUID ID;
	private:
		bool Loaded = true;


	};
}