#pragma once
#include "Chroma/Core/Time.h"
#include "Scene.h"


namespace Chroma
{
	class System
	{
	public:
		System() = default;
		virtual ~System() = default;

		virtual void PreLoad()
		{
		};

		virtual void Load()
		{
		};

		virtual void PostLoad()
		{
		};

		virtual void EarlyInit()
		{
		};

		virtual void Init()
		{
		};

		virtual void LateInit()
		{
		};


		virtual void EarlyUpdate(Time time)
		{
		};

		virtual void Update(Time time)
		{
		};

		virtual void LateUpdate(Time time)
		{
		};

		virtual void EarlyDraw(Time time)
		{
		};

		virtual void Draw(Time time)
		{
		};

		virtual void LateDraw(Time time)
		{
		};


		Scene *GetScene() { return m_Scene; }

	protected:
		friend class Scene;
		Scene *m_Scene;
	};
}
