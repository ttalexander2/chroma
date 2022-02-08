#include "Runtime.h"
#include "Chroma/Utilities/FileDialogs.h"
#include <Chroma/Core/EntryPoint.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Renderer/Renderer2D.h>
#include <Chroma/IO/FileSystem.h>
#include <Chroma/Core/Engine.h>
#include <Chroma/Scripting/MonoScripting.h>
#include <Chroma/Scene/Scene.h>
#include <Chroma/Scripting/ScriptEngineRegistry.h>
#include <Chroma/Components/CSharpScript.h>
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"

using namespace Chroma;


Runtime::Runtime() :
	Application("Chroma Runtime", 1920U, 1080U)
{

}

Runtime::~Runtime()
{
	delete Engine::CurrentScene;
}

void Runtime::Init()
{
		std::string path = "app.info";
		std::string AssetDirectory = "assets";
		std::string StartingScene;
		std::string Name = "Game";
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

		glfwSetWindowTitle((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), Name.c_str());

		try
		{
			Chroma::FileSystem::UnmountAll();
			Chroma::FileSystem::Mount(AssetDirectory);

		}
		catch (Chroma::FileSystem::FileSystemException e)
		{
			CHROMA_CORE_ERROR("{}", e.what());
		}


		Chroma::Engine::LoadScene(StartingScene);

		Chroma::MonoScripting::LoadAppAssembly(std::filesystem::path(path).parent_path().string() + Name + ".dll");
		Chroma::MonoScripting::SetSceneContext(Engine::CurrentScene);

		auto spriteView = Engine::CurrentScene->Registry.view<Chroma::SpriteRenderer>();
		for (Chroma::EntityID e : spriteView)
		{
			auto& spriteRenderer = spriteView.get<Chroma::SpriteRenderer>(e);
			if (spriteRenderer.GetSpriteOrigin() != Chroma::SpriteRenderer::SpriteOrigin::Custom)
				spriteRenderer.SetSpriteOrigin(spriteRenderer.GetSpriteOrigin());
		}


		Chroma::Engine::CurrentScene->OnLoad();
		Chroma::Engine::CurrentScene->Init();
}

void Runtime::Update(Time delta)
	
{
	
		Engine::CurrentScene->Update(delta);

		Renderer2D::Clear();

		Renderer2D::Begin(Engine::CurrentScene->GetPrimaryCamera());

		Engine::CurrentScene->Draw(delta);

		Renderer2D::End();


}


Chroma::Application* Chroma::CreateApplication()
{
	return new Runtime();
}