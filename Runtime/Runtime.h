#pragma once
#include "Chroma.h"

using namespace Chroma;

class Runtime : public Application
{
public:
	Runtime();
	void Update(Time delta) override;

	static Scene* CurrentScene;

	Application* CreateApplication()
	{
		return new Runtime();
	}

};

