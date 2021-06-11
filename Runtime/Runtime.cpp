#include "Runtime.h"
#include "Chroma/Utilities/FileDialogs.h"
#include <Chroma/Core/EntryPoint.h>


Runtime::Runtime() :
	Chroma::Application("Chroma Runtime Loader", 1920U, 1080U)
{
	this->m_ActiveScene = nullptr;
}

void Runtime::Update(Chroma::Time delta)
	
{
	if (this->m_ActiveScene == nullptr)
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
				this->m_ActiveScene = out;
				this->m_ActiveScene->EarlyInit();
				this->m_ActiveScene->Init();
				this->m_ActiveScene->LateInit();
			}
			else
			{
				delete out;
			}
		}
	}
	else
	{
		this->m_ActiveScene->EarlyUpdate(delta);
		this->m_ActiveScene->Update(delta);
		this->m_ActiveScene->LateUpdate(delta);

		static Chroma::OrthographicCameraController cameraController(1920.0f / 1080.0f);

		Chroma::Renderer2D::BeginScene(cameraController.GetCamera());

		this->m_ActiveScene->EarlyDraw(delta);
		this->m_ActiveScene->Draw(delta);
		this->m_ActiveScene->LateDraw(delta);

		Chroma::Renderer2D::EndScene();
	}


}


Chroma::Application* Chroma::CreateApplication()
{
	return new Runtime();
}