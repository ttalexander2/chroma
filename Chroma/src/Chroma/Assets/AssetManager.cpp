#include "chromapch.h"
#include "AssetManager.h"
#include <Chroma/Assets/Sprite.h>
#include <Chroma/Core/Log.h>

namespace Chroma
{


    /// @brief Directory containing assets
    std::string AssetManager::AssetDirectory = ".\\assets";
    
    /// @brief Map containing references to the game's Sprites.
    /// @see AssetID
    /// @see Sprite
    std::unordered_map<size_t, Ref<Sprite>> AssetManager::Sprites;


    /// @brief Hash relative path to Asset to get Asset's ID.
    /// @param path Relative path to Asset in AssetDirectory
    /// @return ID of Asset
    size_t AssetManager::HashPath(const std::string& path)
    {
        return std::hash<std::string>()(path);
    }

    std::string AssetManager::GetPath(const std::string& asset)
    {
        return AssetDirectory + "\\" + asset;
    }

    /// @brief Begin loading of sprites.
    /// 
    /// This function begins the loading phase for sprites. Any sprites already loaded
    /// will be marked as unloaded. LoadSprite() must be called after this function
    /// for every sprite to be loaded.
    /// 
    /// @see LoadSprite()
    /// @see EndSpriteLoad()
    void AssetManager::BeginSpriteLoad()
    {
        for (auto& [id, sprite] : Sprites)
        {
            sprite->Loaded = false;
        }
    }

    /// @brief End loading of sprites.
    /// 
    /// This function ends the loading phase for sprites. Any sprites that were loaded before the begining of the
    /// load phase that have not been loaded durring this phase will be unloaded.
    /// LoadSprite() must be called before this function for every Sprite that should be loaded
    /// (even if the sprite was loaded prior to the loading phase.
    /// 
    /// @see LoadSprite()
    /// @see BeginSpriteLoad()
    void AssetManager::EndSpriteLoad()
    {
        for (auto it = Sprites.begin(); it != Sprites.end();)
        {
            if (!it->second->Loaded)
                it = Sprites.erase(it);
            else
                ++it;
        }
    }

    /// @brief Load a sprite into the AssetManager.
    /// 
    /// This function will load a sprite, if it is not already loaded.
    /// This function must be called during load step in order for the sprite to remain loaded.
    /// Currently supports loading ```.ase```, ```.png```, & ```.jpg``` sprite formats.
    /// 
    /// @param path Relative path of the Sprite.
    /// @return Whether the sprite was loaded succesfully.
    bool AssetManager::LoadSprite(const std::string& path)
    {
        size_t hashval = std::hash<std::string>()(path);
        if (!AssetManager::HasSprite(path))
        {
            AssetManager::Sprites.insert({ hashval, CreateRef<Sprite>(path) });
        }
        bool success = AssetManager::Sprites[hashval]->Load();
        if (success)
            AssetManager::Sprites[hashval]->Loaded = true;

        if (success)
            CHROMA_CORE_TRACE("Sprite Loaded: {}", path);
        else
            CHROMA_CORE_ERROR("Sprite Failed to Load: {}", path);
        return success;
    }


    /// @brief Get a sprite given relative path.
    /// @param path Relative path to the sprite.
    /// @return Reference to the Sprite.
    Ref<Sprite> AssetManager::GetSprite(const std::string& path)
    {
        size_t hashval = std::hash<std::string>()(path);
        return AssetManager::Sprites[hashval];
    }

    /// @brief Check whether a sprite is loaded.
    /// @param path Relative path to the sprite.
    /// @return Returns true if the sprite is loaded.
    bool AssetManager::HasSprite(const std::string& path)
    {
        return Sprites.contains(std::hash<std::string>()(path));
    }

    /// @brief Get map of Sprites
    /// @return Map of sprites.
    std::unordered_map<size_t, Ref<Sprite>>* AssetManager::GetSprites()
    {
        return &AssetManager::Sprites;
    }

}


