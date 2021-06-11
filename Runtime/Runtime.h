#pragma once
#include "Chroma.h"

class Runtime : public Chroma::Application
{
public:
	Runtime();
	void Update(Chroma::Time delta) override;

};

