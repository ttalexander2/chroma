#pragma once

#include "Asset.h"
#include <string>
#include "Chroma/Core/Core.h"
#include <unordered_map>
#include "Sprite.h"

namespace Chroma
{

	typedef size_t AssetID;

	class AssetManager
	{
	public:
		static std::string AssetDirectory;

		static Ref<Sprite> GetSprite(std::string& path);


	private:
		static std::unordered_map<AssetID, Ref<Asset>> Assets;
	};
}