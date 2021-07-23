#pragma once

#include "Asset.h"

#include <string>
#include "Chroma/Core/Core.h"

#include <unordered_map>
#include "Sprite.h"

namespace Chroma
{

	class AssetManager
	{
	public:
		static std::string AssetDirectory;

		static Ref<Sprite> GetSprite(const std::string& path);
		static bool HasSprite(const std::string& path);
		static Ref<Sprite> CreateSprite(const std::string& path);

		static std::unordered_map<size_t, Ref<Sprite>>* GetSprites();

		static std::unordered_map<size_t, Ref<Sprite>> Sprites;
	};
}