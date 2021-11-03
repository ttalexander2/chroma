#include "Project.h"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Scene/Scene.h>
#include "EditorApp.h"
#include "FileWatcherThread.h"

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


		return true;

	}

	
}