#pragma once

#include <Chroma.h>
using namespace Chroma;

class Runtime : public Chroma::Application
{
public:
	Runtime();
	~Runtime();
	void Init() override;
	void Update(Time delta) override;
	void Draw(Time delta) override;
};