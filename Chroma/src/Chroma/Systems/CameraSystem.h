#pragma once
#include "Chroma/Scene/System.h"

namespace Chroma
{
	class CameraSystem : public System
	{
	public:
		void LateInit() override;
		void LateUpdate(Time time) override;
	};
}