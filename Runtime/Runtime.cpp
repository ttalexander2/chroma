#include "Runtime.h"
#include "Chroma/Utilities/FileDialogs.h"
#include <Chroma/Core/EntryPoint.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Renderer/Renderer2D.h>

using namespace Chroma;
Scene* Runtime::CurrentScene = nullptr;


Runtime::Runtime() :
	Application("Chroma Runtime Loader", 1920U, 1080U)
{
	CurrentScene = new Scene();
	auto entity = CurrentScene->NewEntity();
	auto& sprite = entity.AddComponent<SpriteRenderer>();
	AssetManager::LoadSprite("../Polychrome/assets/textures/avi.ase");
	sprite.SetSprite("../Polychrome/assets/textures/avi.ase");
	auto& script = entity.AddComponent<Chroma::LuaScript>();
	script.Path = "../Polychrome/assets/scripts/Test.lua";
	script.ScriptName = "Test.lua";


	CurrentScene->Load();
	CurrentScene->Init();


}

void Runtime::Update(Time delta)
	
{
	if (CurrentScene == nullptr)
	{
		std::string filepath = FileDialogs::OpenFile("Chroma Scene (*.chroma)\0*.chroma\0");
		if (!filepath.empty())
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;
			strStream << stream.rdbuf();
			Scene* out = new Scene();
			if (Scene::Deserialize(out, strStream.str()))
			{
				CurrentScene = out;
				CurrentScene->Init();
			}
			else
			{
				delete out;
			}
		}
	}
	else
	{

		CurrentScene->Update(delta);


		static OrthographicCameraController cameraController(1920.0f / 1080.0f);

		Renderer2D::Clear();

		Renderer2D::Begin(cameraController.GetCamera());

		CurrentScene->Draw(delta);

		//Renderer2D::DrawQuad({ 0,0,0 }, { 2,2,2 }, { 1,1,1,1 });

		Renderer2D::End();
	}


}


Chroma::Application* Chroma::CreateApplication()
{
	return new Runtime();
}