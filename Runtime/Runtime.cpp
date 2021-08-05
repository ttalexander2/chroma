#include "Runtime.h"
#include "Chroma/Utilities/FileDialogs.h"
#include <Chroma/Core/EntryPoint.h>



Chroma::Scene* Runtime::CurrentScene = nullptr;


Runtime::Runtime() :
	Chroma::Application("Chroma Runtime Loader", 1920U, 1080U)
{
	CurrentScene = nullptr;
}

void Runtime::Update(Chroma::Time delta)
	
{
	if (CurrentScene == nullptr)
	{
		std::string filepath = Chroma::FileDialogs::OpenFile("Chroma Scene (*.chroma)\0*.chroma\0");
		if (!filepath.empty())
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;
			strStream << stream.rdbuf();
			Chroma::Scene* out = new Chroma::Scene();
			if (Chroma::Scene::Deserialize(*out, strStream.str()))
			{
				CurrentScene = out;
				CurrentScene->EarlyInit();
				CurrentScene->Init();
				CurrentScene->LateInit();
			}
			else
			{
				delete out;
			}
		}
	}
	else
	{

		CurrentScene->EarlyUpdate(delta);
		CurrentScene->Update(delta);
		CurrentScene->LateUpdate(delta);


		static Chroma::OrthographicCameraController cameraController(1920.0f / 1080.0f);

		Chroma::Renderer2D::BeginScene(cameraController.GetCamera());

		CurrentScene->PreDraw(delta);
		CurrentScene->Draw(delta);
		CurrentScene->PostDraw(delta);

		Chroma::Renderer2D::EndScene();
	}


}


Chroma::Application* Chroma::CreateApplication()
{
	return new Runtime();
}