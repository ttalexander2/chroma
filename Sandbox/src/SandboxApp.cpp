#include <Chroma.h>
#include <imgui.h>



class ExampleLayer : public Chroma::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_TAB))
		{
			CHROMA_TRACE("Tab key is pressed!");
		}
	}

	void OnEvent(Chroma::Event& event) override
	{

	}

	void OnImGuiRender() override
	{

	}
};

class Sandbox : public Chroma::Application
{

public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Chroma::Application* Chroma::CreateApplication()
{
	return new Sandbox();
}