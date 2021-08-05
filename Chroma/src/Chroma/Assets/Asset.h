#pragma once

#include <Chroma/Assets/AssetManager.h>

namespace Chroma
{
	/// @brief Generic game Asset
	///
	/// Interface for game assets. Each asset controlls its name, id, and path.
	/// Any implementing class is required to provide load/unload/reload functionality.
	class Asset
	{
		friend class AssetManager;

	public:
		const std::string Name;
		const std::string GetPath() const { return Path; }
		virtual bool Load() = 0;
		virtual bool Unload() = 0;
		virtual bool Reload() = 0;

	protected:
		std::string Path;
	private:
		bool Loaded = true;


	};
}