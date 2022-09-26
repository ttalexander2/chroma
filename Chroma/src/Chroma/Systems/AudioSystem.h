#pragma once
#include "Chroma/Scene/System.h"

namespace Chroma
{
	class AudioSystem : public System
	{
	public:
		void LateInit() override;
		void LateUpdate(Time delta) override;
	};
}
