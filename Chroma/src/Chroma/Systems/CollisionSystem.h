#pragma once

#include "Chroma/Scene/System.h"

namespace Chroma
{
	class CollisionSystem : public System
	{
	public:
		void Init() override;
		void Update(Time time) override;


	};
}