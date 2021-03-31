#pragma once
#include "Chroma/Core/Time.h"
#include "Scene.h"

namespace Chroma
{
	class System
	{
	public:
		virtual void EarlyInit() = 0;
		virtual void Init() = 0;
		virtual void LateInit() = 0;


		virtual void EarlyUpdate(Time time) = 0;
		virtual void Update(Time time) = 0;
		virtual void LateUpdate(Time time) = 0;

		virtual void EarlyDraw(Time time) = 0;
		virtual void Draw(Time time) = 0;
		virtual void LateDraw(Time time) = 0;


		inline Scene* GetScene() { return m_Scene; }

	private:
		Scene* m_Scene;
	};
}