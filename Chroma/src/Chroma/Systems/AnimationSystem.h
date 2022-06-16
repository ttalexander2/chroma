#pragma once


#pragma once
#include "Chroma/Scene/System.h"

namespace Chroma
{
	class AnimationSystem : public System
	{
	public:
		void Init() override;
		void Update(Time delta) override;
	};
}

