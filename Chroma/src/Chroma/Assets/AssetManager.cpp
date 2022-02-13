#include "chromapch.h"
#include "AssetManager.h"
#include <Chroma/Assets/Asset.h>
#include <Chroma/Core/Log.h>

namespace Chroma
{
    
    /// @brief Map containing references to the game's assets.
    /// @see AssetID
    /// @see Sprite
    std::unordered_map<size_t, Ref<Asset>> AssetManager::s_Assets;
    std::unordered_map<StringHash, std::vector<size_t>> AssetManager::s_AssetTypes;
    std::hash<std::string> AssetManager::s_Hash;


    Ref<Asset> AssetManager::Get(const std::string& assetPath)
    {
        if (Exists(assetPath))
        {
            return (s_Assets[s_Hash(assetPath)]);
        }
        return nullptr;
    }


    bool AssetManager::Exists(const std::string& id)
    {
        return s_Assets.contains(s_Hash(id));
    }


    bool AssetManager::Load(const std::string& id)
    {
        if (!Exists(id))
        {
            CHROMA_CORE_WARN("Asset cannot be loaded, as it does not yet exist! {}", id);
            return false;
        }
            

        auto& asset = s_Assets[s_Hash(id)];

        if (asset->IsLoaded())
        {
            CHROMA_CORE_WARN("Asset is already loaded! {}", id);
            return false;
        }

        bool result = asset->Load();
        asset->m_Loaded = true;
        return result;
    }


    bool AssetManager::Unload(const std::string& id)
    {
        if (!Exists(id))
            return false;
        auto& asset = s_Assets[s_Hash(id)];

        if (!asset->IsLoaded())
            return false;

        return asset->Unload();
    }


    bool AssetManager::Reload(const std::string& id)
    {
        if (!Exists(id))
            return false;
        auto& asset = s_Assets[s_Hash(id)];

        if (asset->IsLoaded())
            return asset->Unload();

        return asset->Load();
    }

    bool AssetManager::IsLoaded(const std::string& assetPath)
    {
        return Exists(assetPath) && s_Assets[s_Hash(assetPath)]->IsLoaded();
    }

    void AssetManager::UnloadAll()
    {
        std::vector<size_t> toRemove;
        for (auto& [hash, asset] : s_Assets)
        {
            asset->Unload();
            asset.reset();
            toRemove.push_back(hash);
        }
        for (size_t hash : toRemove)
        {
            s_Assets.erase(s_Assets.find(hash));
        }
    }

    void AssetManager::UnloadUnused()
    {
        std::vector<size_t> toRemove;
        for (auto& [hash, asset] : s_Assets)
        {
            if (asset.use_count() < 1)
            {
                asset->Unload();
                asset.reset();
                toRemove.push_back(hash);
            }
        }
        for (size_t hash : toRemove)
        {
            s_Assets.erase(s_Assets.find(hash));
        }
    }

}