#include "chromapch.h"
#include "AssetManager.h"

namespace Chroma
{
    Ref<Sprite> Chroma::AssetManager::GetSprite(std::string& path)
    {
        Ref<Asset> asset = Assets[std::hash<std::string>()(path)];
        Ref<Sprite> sprite = std::dynamic_pointer_cast<Sprite>(asset);
        if (sprite.get() == nullptr)
            CHROMA_CORE_WARN("Warning: [{0}] is either missing or not a sprite.");
        return sprite;
    }
}


