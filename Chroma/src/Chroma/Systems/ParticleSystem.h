#pragma once

#include "Chroma/Scene/System.h"
#include "Chroma/Components/ParticleEmitter.h"

namespace Chroma
{

	class ParticleSystem : public System
	{
	public:
		void Init() override;
		void Update(Time detla) override;
		void Draw(Time delta) override;

		static void UpdateEmitter(ParticleEmitter& emitter, glm::vec2 position, Time delta);
	};

}