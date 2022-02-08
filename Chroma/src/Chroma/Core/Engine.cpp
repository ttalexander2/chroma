#include "chromapch.h"
#include "Engine.h"

#include "Chroma/Assets/AssetManager.h"

namespace Chroma
{
    Scene* Engine::CurrentScene = nullptr;

    bool Engine::LoadScene(const std::string& sceneFile)
    {
        File f = FileSystem::Open(sceneFile);
        if (!f.Good())
        {
            CHROMA_CORE_ERROR("Failed to read file {[]}", sceneFile);
            f.Close();
            return false;
        }
        bool result = LoadScene(f);
        f.Close();

        return result;
    }

    bool Engine::LoadScene(File& file)
    {
        Scene* out = new Scene();
        
        std::string yaml = file.ReadString();
        bool result = out->Deserialize(out, yaml);
        if (!result)
        {
            CHROMA_CORE_ERROR("Scene {[]} failed to load!", file.GetPath());
            return false;
        }
        if (Engine::CurrentScene != nullptr)
            delete Engine::CurrentScene;

        Engine::CurrentScene = out;

        //AssetManager::UnloadUnused();

        return true;
    }
}


