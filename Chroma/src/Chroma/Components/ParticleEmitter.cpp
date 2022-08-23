#include "chromapch.h"
#include "ParticleEmitter.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	std::random_device ParticleEmitter::sRd;
	std::default_random_engine ParticleEmitter::sEngine { sRd() };
	std::uniform_int_distribution<long long> ParticleEmitter::sDist{ std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max() };
	
	void ParticleEmitter::GenerateRandomSeed()
	{
		SetSeed(sDist(sEngine));
	}

	void ParticleEmitter::Emit(const Math::vec2& position)
	{
		Particle& p = Particles[pos];
		p.Active = true;
		p.Angle = this->Angle + ((random() - 0.5f) * 2 * this->AngleVariance);
		p.Life = this->Life + ((random() - 0.5f) * 2 * this->LifeVariance);
		p.Position = position;
		p.Speed = this->Speed + ((random() - 0.5f) * 2 * this->SpeedVariance);
		p.TotalLife = p.Life;

		pos = (pos + 1) % Particles.size();
	}

	Reflection::TypeFactory<ParticleEmitter> ParticleEmitter::RegisterType()
	{
		Reflection::Register<Particle>("Particle")
				.Data<&Particle::Active>("Active")
				.Data<&Particle::Position>("Position")
				.Data<&Particle::Life>("Life")
				.Data<&Particle::TotalLife>("TotalLife")
				.Data<&Particle::Speed>("Speed")
				.Data<&Particle::Angle>("Angle");

		return Reflection::Register<ParticleEmitter>("ParticleEmitter")
				.Base<Component>()
				.Data<&ParticleEmitter::PositionVariance>("PositionVariance")
				.Data<&ParticleEmitter::StartColor>("StartColor")
				.Data<&ParticleEmitter::EndColor>("EndColor")
				.Data<&ParticleEmitter::Life>("Life")
				.Data<&ParticleEmitter::LifeVariance>("LifeVariance")
				.Data<&ParticleEmitter::Angle>("Angle")
				.Data<&ParticleEmitter::AngleVariance>("AngleVariance")
				.Data<&ParticleEmitter::Speed>("Speed")
				.Data<&ParticleEmitter::SpeedVariance>("SpeedVariance")
				.Data<&ParticleEmitter::Gravity>("Gravity")
				.Data<&ParticleEmitter::GravityVariance>("GravityVariance")
				.Data<&ParticleEmitter::Continuous>("Continuous")
				.Data<&ParticleEmitter::EmissionRate>("EmissionRate")
				.Data<&ParticleEmitter::PreWarm>("PreWarm")
				.Data<&ParticleEmitter::PreWarmSeconds>("PreWarmSeconds")
				.Data<&ParticleEmitter::SetSeed, &ParticleEmitter::GetSeed>("Seed")
				.Data<&ParticleEmitter::SetCount, &ParticleEmitter::GetCount>("Count");
	}

	float ParticleEmitter::random()
	{
		return dist(engine);
	}

}