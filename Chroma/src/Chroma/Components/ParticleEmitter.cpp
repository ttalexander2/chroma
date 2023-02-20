#include "chromapch.h"
#include "ParticleEmitter.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	std::random_device ParticleEmitter::sRd;
	std::default_random_engine ParticleEmitter::sEngine{ sRd() };
	std::uniform_int_distribution<unsigned int> ParticleEmitter::sDist{ std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max() };

	void ParticleEmitter::GenerateRandomSeed()
	{
		SetSeed(sDist(sEngine));
	}

	void ParticleEmitter::Emit(const Math::vec2 &position)
	{
		Particle &p = Particles[pos];
		p.Active = true;
		p.Angle = this->Angle + ((random() - 0.5f) * 2 * this->AngleVariance);
		p.Life = this->Life + ((random() - 0.5f) * 2 * this->LifeVariance);
		p.Position = position;
		p.Speed = this->Speed + ((random() - 0.5f) * 2 * this->SpeedVariance);
		p.TotalLife = p.Life;

		pos = (pos + 1) % Particles.size();
	}

	Reflection::type_factory<ParticleEmitter> ParticleEmitter::register_type()
	{
		Reflection::register_type<Particle>("Particle")
				.data<&Particle::Active>("Active")
				.data<&Particle::Position>("Position")
				.data<&Particle::Life>("Life")
				.data<&Particle::TotalLife>("TotalLife")
				.data<&Particle::Speed>("Speed")
				.data<&Particle::Angle>("Angle");

		return Reflection::RegisterComponent<ParticleEmitter>("ParticleEmitter")
		       .base<Component>()
		       .data<&ParticleEmitter::PositionVariance>("PositionVariance")
		       .data<&ParticleEmitter::StartColor>("StartColor")
		       .data<&ParticleEmitter::EndColor>("EndColor")
		       .data<&ParticleEmitter::Life>("Life")
		       .data<&ParticleEmitter::LifeVariance>("LifeVariance")
		       .data<&ParticleEmitter::Angle>("Angle")
		       .data<&ParticleEmitter::AngleVariance>("AngleVariance")
		       .data<&ParticleEmitter::Speed>("Speed")
		       .data<&ParticleEmitter::SpeedVariance>("SpeedVariance")
		       .data<&ParticleEmitter::Gravity>("Gravity")
		       .data<&ParticleEmitter::GravityVariance>("GravityVariance")
		       .data<&ParticleEmitter::Continuous>("Continuous")
		       .data<&ParticleEmitter::EmissionRate>("EmissionRate")
		       .data<&ParticleEmitter::PreWarm>("PreWarm")
		       .data<&ParticleEmitter::PreWarmSeconds>("PreWarmSeconds")
		       .data<&ParticleEmitter::GetSeed, &ParticleEmitter::SetSeed>("Seed")
		       .data<&ParticleEmitter::GetCount, &ParticleEmitter::SetCount>("Count");
	}

	float ParticleEmitter::random()
	{
		return dist(engine);
	}
}
