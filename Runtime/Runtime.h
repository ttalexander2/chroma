#pragma once

#include <Chroma.h>
using namespace Chroma;

class Runtime : public Chroma::Application
{
public:
	static Scene* CurrentScene;
	Runtime();
	~Runtime();
	void Update(Time delta) override;
};