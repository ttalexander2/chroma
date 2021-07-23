#include "chromapch.h"
#include "AssetManager.h"
#include <Chroma/Assets/Sprite.h>

namespace Chroma
{

    std::string AssetManager::AssetDirectory = ".\\assets";
    std::unordered_map<size_t, Ref<Sprite>> AssetManager::Sprites;

   
    Ref<Sprite> AssetManager::GetSprite(const std::string& path)
    {
        size_t hashval = std::hash<std::string>()(path);
        return AssetManager::Sprites[hashval];
    }


    Ref<Sprite> AssetManager::CreateSprite(const std::string& path)
    {
        size_t hashval = std::hash<std::string>()(path);
        AssetManager::Sprites.insert({ hashval, CreateRef<Sprite>(path) });
        return AssetManager::Sprites[hashval];
    }

    bool AssetManager::HasSprite(const std::string& path)
    {
        return Sprites.contains(std::hash<std::string>()(path));
    }

    std::unordered_map<size_t, Ref<Sprite>>* AssetManager::GetSprites()
    {
        return &AssetManager::Sprites;
    }

}


