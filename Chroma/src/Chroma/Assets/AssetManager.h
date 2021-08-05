#pragma once

#include "Asset.h"

#include <string>
#include "Chroma/Core/Core.h"

#include <unordered_map>

namespace Chroma
{

	class Sprite;

	/// @brief Manages loading/unloading assets.
	///
	/// Class for managing assets during runtime. Handles loading & unloading of assets.
	/// Manages references to assets.
	class AssetManager
	{


	public:

		static std::string AssetDirectory;

		static void BeginSpriteLoad();
		static void EndSpriteLoad();

		static Ref<Sprite> GetSprite(const std::string& path);
		static bool HasSprite(const std::string& path);
		static bool LoadSprite(const std::string& path);

		static size_t HashPath(const std::string& path);

		static std::unordered_map<size_t, Ref<Sprite>>* GetSprites();

		static std::unordered_map<size_t, Ref<Sprite>> Sprites;
	};
}