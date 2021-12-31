#include "Project.h"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Scene/Scene.h>
#include "EditorApp.h"
#include "FileWatcherThread.h"
#include "Build.h"
#include <Chroma/Scripting/ScriptEngineRegistry.h>
#include <Chroma/Scripting/MonoScripting.h>
#include <Chroma/IO/FileSystem.h>
#include <Chroma/Components/CSharpScript.h>

namespace Polychrome
{

	std::string Project::StartingScene;
	std::string Project::Name;
	std::string Project::Path;

	bool Project::CreateProject(const std::string& name, const std::string& directory, const std::string& starting_scene)
	{
		if (!std::filesystem::exists(directory))
			std::filesystem::create_directory(directory);
		auto final_dir = directory + "\\" + name;
		std::filesystem::create_directories(final_dir);

		Path = final_dir;
		Name = name;
		StartingScene = starting_scene;

		YAML::Emitter e;
		e << YAML::BeginMap;
		e << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;
		e << YAML::Key << "Name" << YAML::Value << name;
		e << YAML::Key << "AssetDirectory" << YAML::Value << "Assets";
		e << YAML::Key << "ScriptModules" << YAML::Value << "Modules";
		e << YAML::Key << "StartingScene" << YAML::Value << starting_scene + ".chroma";
		Chroma::World::Serialize(e);
		e << YAML::EndMap << YAML::EndMap;

		std::string path = final_dir + "/" + name + ".polychrome";
		std::ofstream stream (path, std::ofstream::out);
		stream.write(e.c_str(), e.size());
		stream.close();

		//Create Directories
		std::filesystem::create_directory(final_dir + "/Assets");
		std::filesystem::create_directory(final_dir + "/Assets/Fmod");
		std::filesystem::create_directory(final_dir + "/Assets/Shaders");
		std::filesystem::create_directory(final_dir + "/Assets/Textures");
		std::filesystem::create_directory(final_dir + "/Assets/Scripts");
		std::filesystem::create_directory(final_dir + "/Assets/Scenes");

		stream = std::ofstream(final_dir + "/" + ".gitignore");
		stream.write("obj\nbin", sizeof("obj\nbin")/sizeof(char));
		stream.close();

		LoadProject(path);

		return true;

	}

	bool Project::SaveCurrentProject()
	{

		YAML::Emitter e;
		e << YAML::BeginMap;
		e << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;
		e << YAML::Key << "Name" << YAML::Value << Name;
		e << YAML::Key << "AssetDirectory" << YAML::Value << "Assets";
		e << YAML::Key << "ScriptModules" << YAML::Value << "Modules";
		e << YAML::Key << "StartingScene" << YAML::Value << StartingScene;
		Chroma::World::Serialize(e);
		e << YAML::EndMap << YAML::EndMap;

		auto final_dir = Path + "\\" + Name;
		std::string path = final_dir + ".polychrome";
		//CHROMA_CORE_TRACE("Path: {}", path);
		std::ofstream stream(path, std::ofstream::out);
		if (!stream.good())
			return false;
		stream.write(e.c_str(), e.size());
		stream.close();
		return true;
	}

	bool Project::LoadProject(const std::string& path)
	{
		auto ss = std::ostringstream{};
		std::ifstream stream(path);
		if (stream.good())
		{
			ss << stream.rdbuf();
			auto yaml = YAML::Load(ss.str());
			auto project = yaml["Project"];
			if (project)
			{

				if (project["Name"])
				{
					Name = project["Name"].as<std::string>();
				}
				if (project["AssetDirectory"])
				{
					Chroma::AssetManager::AssetDirectory = std::filesystem::path(path).parent_path().string() + "\\" + project["AssetDirectory"].as<std::string>();
				}
				if (project["StartingScene"])
				{
					StartingScene = project["StartingScene"].as<std::string>();
				}
				if (project["Layers"])
				{
					Chroma::World::Deserialize(project);
				}

			}
		}
		else
		{
			return false;
		}
		stream.close();

		Path = std::filesystem::path(path).parent_path().string();

		if (!StartingScene.empty() && std::filesystem::exists(Chroma::AssetManager::AssetDirectory + "\\" + StartingScene))
		{
			ss = std::ostringstream{};
			std::ifstream stream(Chroma::AssetManager::AssetDirectory + "\\" + StartingScene);
			ss << stream.rdbuf();
			Chroma::Scene* scene = new Chroma::Scene();
			Chroma::Scene::Deserialize(scene, ss.str());
			EditorApp::CurrentScene = scene;
			EditorApp::CurrentScenePath = Chroma::AssetManager::AssetDirectory + StartingScene;
		}
		else
		{
			EditorApp::CurrentScene = new Chroma::Scene();
		}

		std::string proj = R"(<Project Sdk="Microsoft.NET.Sdk">

	<PropertyGroup>
		<TargetFramework>net4.7</TargetFramework>
	</PropertyGroup>

	<ItemGroup>
		<Reference Include = "Chroma.Mono">
			<HintPath>)" + std::filesystem::current_path().string() + "\\Chroma.Mono.dll" + R"(</HintPath>
		</Reference>
	</ItemGroup>

</Project>
		)";

		auto outStream = std::ofstream(Path + "/" + Name + ".csproj");
		outStream.write(proj.c_str(), proj.length());
		outStream.close();

		//SaveCurrentProject();

		for (auto& file : std::filesystem::recursive_directory_iterator(Chroma::AssetManager::AssetDirectory))
		{
			if (file.is_regular_file())
			{
				//CHROMA_CORE_INFO("{}", file.path().string());
				std::string extension = file.path().extension().string();
				if (extension == ".ase" || extension == ".aseprite" || extension == ".png" || extension == ".jpg")
				{
					Chroma::AssetManager::LoadSprite(file.path().lexically_relative(Chroma::AssetManager::AssetDirectory).string());
					//CHROMA_CORE_INFO("file: {}", file.path().string());
				}
			}
		}

		auto result = Build::BuildMonoAssembly(std::filesystem::path(Chroma::AssetManager::AssetDirectory).parent_path().string(), Project::Name);
		Chroma::ScriptEngineRegistry::RegisterAll();
		Chroma::MonoScripting::SetDeltaTime(0.f, 0.f);
		Chroma::MonoScripting::SetSceneContext(EditorApp::CurrentScene);
		auto view = EditorApp::CurrentScene->Registry.view<Chroma::CSharpScript>();

		for (Chroma::EntityID entity : view)
		{
			auto& script = view.get<Chroma::CSharpScript>(entity);
			auto entityObj = Chroma::Entity(entity, EditorApp::CurrentScene);
			Chroma::MonoScripting::InitScriptEntity(entityObj);
		}

		auto spriteView = EditorApp::CurrentScene->Registry.view<Chroma::SpriteRenderer>();
		for (Chroma::EntityID e : spriteView)
		{
			auto& spriteRenderer = spriteView.get<Chroma::SpriteRenderer>(e);
			if (spriteRenderer.GetSpriteOrigin() != Chroma::SpriteRenderer::SpriteOrigin::Custom)
				spriteRenderer.SetSpriteOrigin(spriteRenderer.GetSpriteOrigin());
		}

		//CHROMA_CORE_WARN("Mounted: {}", Chroma::AssetManager::AssetDirectory);
		//CHROMA_CORE_WARN("Mounted: {}", std::filesystem::path(Chroma::AssetManager::AssetDirectory).parent_path().string() + "\\TestArchive.7z");
		//CHROMA_CORE_WARN("");

		try
		{
			Chroma::FileSystem::UnmountAll();
			Chroma::FileSystem::Mount(Chroma::AssetManager::AssetDirectory);
			//Chroma::FileSystem::Mount(std::filesystem::path(Chroma::AssetManager::AssetDirectory).parent_path().string() + "\\TestArchive.7z");

		}
		catch (Chroma::FileSystem::FileSystemException e)
		{
			CHROMA_CORE_ERROR("{}", e.what());
		}

		//for (auto& path : Chroma::FileSystem::GetFileListRecursive(""))
		//{
		//	CHROMA_CORE_WARN("{}", path);
		//}


		return true;

	}

	
}