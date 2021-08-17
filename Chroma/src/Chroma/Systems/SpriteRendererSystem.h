#pragma once

#include "Chroma/Scene/System.h"

namespace Polychrome
{
	class EditorApp;
}

namespace Chroma
{
	class SpriteRendererSystem : public System
	{
	public:
		void Load() override;
		void Init() override;
		void Draw(Time delta) override;
		virtual void LateUpdate(Time delta) override;

		friend class Polychrome::EditorApp;

	};

}


