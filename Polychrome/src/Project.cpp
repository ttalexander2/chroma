#include "Project.h"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>

namespace Polychrome
{

	bool Project::CreateProject(const std::string& name, const std::string& directory)
	{
		if (!std::filesystem::exists(directory))
			std::filesystem::create_directory(directory);

		YAML::Emitter e;
		e << YAML::BeginMap;
		e << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;
		e << YAML::Key << "Name" << YAML::Value << name;
		e << YAML::Key << "AssetDirectory" << YAML::Value << "Assets";
		e << YAML::Key << "ScriptModules" << YAML::Value << "Modules";
		e << YAML::EndMap << YAML::EndMap;

		std::string path = directory + "/" + name + ".chproj";
		std::ofstream stream (path, std::ofstream::out);
		stream.write(e.c_str(), e.size());
		stream.close();

		//Create Directories
		std::filesystem::create_directory(directory + "/Assets");
		std::filesystem::create_directory(directory + "/Assets/Fmod");
		std::filesystem::create_directory(directory + "/Assets/Shaders");
		std::filesystem::create_directory(directory + "/Assets/Textures");
		std::filesystem::create_directory(directory + "/Assets/Scripts");

		stream = std::ofstream(directory + "/" + ".gitignore");
		stream.write("obj\nbin", sizeof("obj\nbin")/sizeof(char));
		stream.close();

		std::string proj = R"(
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <TargetFramework>net5.0</TargetFramework>
  </PropertyGroup>

  <ItemGroup>
	<Reference Include = "Chroma.Mono">
		<HintPath>)" + std::filesystem::current_path().string() + "/Chroma.Mono.dll" + R"(</HintPath>
	</Reference>
  </ItemGroup>

</Project>
)";

		stream = std::ofstream(directory + "/" + name + ".csproj");
		stream.write(proj.c_str(), proj.length());
		stream.close();

		//this is sus
		//system(("dotnet new classlib --output " + directory + " --name " + name).c_str());
		//system(("dotnet add " + directory + "/" + name + ".csproj" + " reference Chroma.Mono.LINK.csproj").c_str());

		return true;

	}

	bool Project::LoadProject(const std::string& path)
	{
		return false;
	}
}