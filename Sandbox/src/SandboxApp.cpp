#include <Chroma.h>


class Sandbox : public Chroma::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Chroma::Application* Chroma::CreateApplication()
{
	return new Sandbox();
}