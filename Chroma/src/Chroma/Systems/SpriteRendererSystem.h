#pragma once

#include "Chroma/Scene/System.h"

namespace Chroma
{
	class SpriteRendererSystem : public System
	{
	public:
		void Init() override;
		void Draw(Time delta) override;
	};

}


