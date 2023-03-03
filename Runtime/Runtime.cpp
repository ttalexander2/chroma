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
#include <Chroma/Assets/AssetManager.h>
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"

using namespace Chroma;

static Chroma::Ref<Chroma::Font> test_font;
static Chroma::Ref<Chroma::Shader> test_shader;


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
		//CHROMA_CORE_TRACE("working directory: {}", std::filesystem::current_path());
		//CHROMA_CORE_TRACE("shader file: {}", std::filesystem::current_path().string() + "/assets/Shaders/Font.glsl");
		//test_shader = Chroma::Shader::Create(std::filesystem::current_path().string() + "/assets/Shaders/Font.glsl");


		std::string path = "app.info";
		std::string AssetDirectory = "assets";
		std::string StartingScene;
		std::string Name = "Game";
		auto ss = std::ostringstream{};
		std::ifstream stream(path);

		if (!std::filesystem::exists(path))
		{
			this->Stop();
			return;
		}
			
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
			Chroma::FileSystem::SetWriteDirectory(AssetDirectory);

		}
		catch (Chroma::FileSystem::FileSystemException e)
		{
			CHROMA_CORE_ERROR("{}", e.what());
			this->Stop();
		}

		if (!std::filesystem::exists("Assets.yaml"))
		{
			CHROMA_CORE_ERROR("Missing Assets.yaml!");
			this->Stop();
		}


		std::ifstream asset_stream("Assets.yaml");
		std::stringstream asset_ss;
		asset_ss << asset_stream.rdbuf();
		Chroma::AssetManager::LoadManifest(asset_ss.str());
		asset_stream.close();

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

		//test_font = Chroma::AssetManager::Get<Chroma::Font>(Chroma::GUID::Parse("85519709-783D-4406-BCA3-C69A95030087"));
		//Chroma::AssetManager::Load(test_font->GetID());
}

void Runtime::Update(Time delta)
{
		Engine::CurrentScene->Update(delta);
}

void Runtime::Draw(Time delta)
{
	Renderer2D::Clear();

	Renderer2D::Begin(Engine::CurrentScene->GetPrimaryCamera());

	Engine::CurrentScene->Draw(delta);

	Renderer2D::End();

	//Renderer2D::Begin(Engine::CurrentScene->GetPrimaryCamera());
	//Chroma::Renderer2D::DrawString("Among Us 2: Sussy Baka", test_font, Math::vec3(90, 0, 0), 5, Math::vec4(1, 1, 1, 1), 24.f, 0, 0);
	//Renderer2D::Flush(test_shader);
}


Chroma::Application* Chroma::CreateApplication()
{
	return new Runtime();
}