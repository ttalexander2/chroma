#include "chromapch.h"
#include "AssetManager.h"
#include <Chroma/Assets/Asset.h>
#include <Chroma/Core/Log.h>
#include <Chroma/IO/File.h>
#include <Chroma/IO/FileSystem.h>

#include <yaml-cpp/yaml.h>

namespace Chroma
{
    
    /// @brief Map containing references to the game's assets.
    /// @see AssetID
    /// @see Sprite
    std::unordered_map<GUID, Ref<Asset>> AssetManager::s_Assets;
    std::unordered_map<StringHash, std::vector<GUID>> AssetManager::s_AssetTypes;
    std::unordered_map<size_t, GUID> AssetManager::s_Paths;
    std::unordered_map<size_t, std::function<void(const GUID&, const std::string&)>> AssetManager::s_CreateFuncs;
    std::unordered_map<size_t, std::string> AssetManager::s_Extensions = {{s_Hash(".png"), Sprite::GetTypeNameStatic()}, {s_Hash(".ase"), Sprite::GetTypeNameStatic()}, {s_Hash(".aseprite"), Sprite::GetTypeNameStatic()}};
    std::hash<std::string> AssetManager::s_Hash;


    Ref<Asset> AssetManager::Get(const GUID& id)
    {
        if (Exists(id))
        {
            return (s_Assets[id]);
        }
        CHROMA_CORE_ERROR("Sprite {} does not exist!", id.ToString());
        return nullptr;
    }


    Ref<Asset> AssetManager::Create(const GUID& id, const std::string& path, const std::string& type)
    {
        if (Exists(id))
        {
            return Get(id);
        }

        s_CreateFuncs[s_Hash(type)](id, path);
        return Get(id);
    }

    bool AssetManager::Exists(const GUID& id)
    {
        return s_Assets.contains(id);
    }


    bool AssetManager::Load(const GUID& id)
    {
        if (!Exists(id))
        {
            CHROMA_CORE_WARN("Asset cannot be loaded, as it does not yet exist! {}", id.ToString());
            return false;
        }
            

        auto& asset = s_Assets[id];

        if (asset->IsLoaded())
        {
            CHROMA_CORE_WARN("Asset is already loaded! {}", id.ToString());
            return false;
        }

        bool result = asset->Load();
        asset->m_Loaded = true;
        return result;
    }


    bool AssetManager::Unload(const GUID& id)
    {
        if (!Exists(id))
            return false;
        auto& asset = s_Assets[id];

        if (!asset->IsLoaded())
            return false;

        return asset->Unload();
    }


    bool AssetManager::Reload(const GUID& id)
    {
        if (!Exists(id))
            return false;
        auto& asset = s_Assets[id];

        if (asset->IsLoaded())
            return asset->Unload();

        return asset->Load();
    }

    bool AssetManager::IsLoaded(const GUID& id)
    {
        return Exists(id) && s_Assets[id]->IsLoaded();
    }

    void AssetManager::UnloadAll()
    {
        for (auto& [hash, asset] : s_Assets)
        {
            asset->Unload();
        }
    }

    void AssetManager::UnloadUnused()
    {
        for (auto& [hash, asset] : s_Assets)
        {
            if (asset.use_count() < 1)
            {
                asset->Unload();
            }
        }
    }

    GUID AssetManager::GetID(const std::string& path)
    {
        if (s_Paths.contains(s_Hash(path)))
        {
            return s_Paths[s_Hash(path)];
        }
        return GUID::Zero();
    }

    void AssetManager::LoadManifest(const std::string& yaml)
    {
        auto data = YAML::Load(yaml);
        if (data && data.IsMap())
        {
            for (auto item : data)
            {
                GUID id = GUID::Parse(item.first.as<std::string>());
                if (item.second && item.second.IsMap())
                {
                    std::string path;
                    auto val = item.second["Path"];
                    if (val)
                    {
                        path = val.as<std::string>();
                        s_Paths[s_Hash(path)] = id;
                    }

                    val = item.second["Type"];
                    if (val)
                    {
                        std::string type = val.as<std::string>();
                        Create(id, path, type);

                    }
                }
            }
        }
    }

    std::string AssetManager::SaveManifest()
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        for (auto& [hash, asset] : s_Assets)
        {
	        out << YAML::Key << hash.ToString();
            out << YAML::Value;
            out << YAML::BeginMap;

            out << YAML::Key << "Path";
            out << YAML::Value << asset->m_Path;
            out << YAML::Key << "Type";
            out << YAML::Value << asset->GetTypeName();


            out << YAML::EndMap;
        }

        out << YAML::EndMap;


        return out.c_str();
    }

}