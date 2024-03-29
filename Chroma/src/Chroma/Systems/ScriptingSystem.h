#pragma once

#include "Chroma/Scene/System.h"


namespace Chroma
{
	class ScriptingSystem : public System
	{
	public:

		ScriptingSystem() {}

		void Load() override;

		void EarlyInit() override;
		void Init() override;
		void LateInit() override;

		void EarlyUpdate(Time time) override;
		void Update(Time time) override;
		void LateUpdate(Time time) override;


	};
}