#include "chromapch.h"
#include "ParticleEmitter.h"

namespace Chroma
{
	std::random_device ParticleEmitter::sRd;
	std::default_random_engine ParticleEmitter::sEngine { sRd() };
	std::uniform_int_distribution<long long> ParticleEmitter::sDist{ std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max() };

	void ParticleEmitter::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "PositionVariance";
		out << YAML::Value << PositionVariance;

		out << YAML::Key << "StartColor";
		out << YAML::Value << StartColor;

		out << YAML::Key << "EndColor";
		out << YAML::Value << EndColor;

		out << YAML::Key << "Life";
		out << YAML::Value << Life;

		out << YAML::Key << "LifeVariance";
		out << YAML::Value << LifeVariance;

		out << YAML::Key << "Angle";
		out << YAML::Value << Angle;

		out << YAML::Key << "AngleVariance";
		out << YAML::Value << AngleVariance;

		out << YAML::Key << "Speed";
		out << YAML::Value << Speed;

		out << YAML::Key << "SpeedVariance";
		out << YAML::Value << SpeedVariance;

		out << YAML::Key << "Gravity";
		out << YAML::Value << Gravity;

		out << YAML::Key << "GravityVariance";
		out << YAML::Value << GravityVariance;

		out << YAML::Key << "Continuous";
		out << YAML::Value << Continuous;

		out << YAML::Key << "EmissionRate";
		out << YAML::Value << EmissionRate;

		out << YAML::Key << "PreWarm";
		out << YAML::Value << PreWarm;

		out << YAML::Key << "PreWarmSeconds";
		out << YAML::Value << PreWarmSeconds;

		out << YAML::Key << "Seed";
		out << YAML::Value << _seed;

		out << YAML::Key << "Count";
		out << YAML::Value << Particles.size();
	}
	void ParticleEmitter::Deserialize(YAML::Node& node)
	{
		auto val = node["PositionVariance"];
		if (val)
		{
			PositionVariance = val.as<Math::vec2>();
		}

		val = node["StartColor"];
		if (val)
		{
			StartColor = val.as<Math::vec4>();
		}

		val = node["EndColor"];
		if (val)
		{
			EndColor = val.as<Math::vec4>();
		}

		val = node["Life"];
		if (val)
		{
			Life = val.as<float>();
		}

		val = node["LifeVariance"];
		if (val)
		{
			LifeVariance = val.as<float>();
		}

		val = node["Angle"];
		if (val)
		{
			Angle = val.as<float>();
		}

		val = node["AngleVariance"];
		if (val)
		{
			AngleVariance = val.as<float>();
		}

		val = node["Speed"];
		if (val)
		{
			Speed = val.as<float>();
		}

		val = node["SpeedVariance"];
		if (val)
		{
			SpeedVariance = val.as<float>();
		}

		val = node["Gravity"];
		if (val)
		{
			Gravity = val.as<float>();
		}

		val = node["GravityVariance"];
		if (val)
		{
			GravityVariance = val.as<float>();
		}

		val = node["Continuous"];
		if (val)
		{
			Continuous = val.as<bool>();
		}

		val = node["EmissionRate"];
		if (val)
		{
			EmissionRate = val.as<float>();
		}

		val = node["PreWarm"];
		if (val)
		{
			PreWarm = val.as<bool>();
		}

		val = node["PreWarmSeconds"];
		if (val)
		{
			PreWarmSeconds = val.as<float>();
		}

		val = node["Seed"];
		if (val)
		{
			_seed = val.as<long long>();
		}

		val = node["Count"];
		if (val)
		{
			size_t count = val.as<size_t>();
			Particles.resize(count);
		}
	}
	
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

	float ParticleEmitter::random()
	{
		return dist(engine);
	}

	void ParticleEmitter::CreateReflectionModel()
	{
		entt::meta<ParticleEmitter>()
			.data<&ParticleEmitter::PositionVariance>("PositionVariance"_hs)
			.data<&ParticleEmitter::StartColor>("StartColor"_hs)
			.data<&ParticleEmitter::EndColor>("EndColor"_hs)
			.data<&ParticleEmitter::Life>("Life"_hs)
			.data<&ParticleEmitter::LifeVariance>("LifeVariance"_hs)
			.data<&ParticleEmitter::Angle>("Angle"_hs)
			.data<&ParticleEmitter::AngleVariance>("AngleVariance"_hs)
			.data<&ParticleEmitter::Speed>("Speed"_hs)
			.data<&ParticleEmitter::SpeedVariance>("SpeedVariance"_hs)
			.data<&ParticleEmitter::Gravity>("Gravity"_hs)
			.data<&ParticleEmitter::GravityVariance>("GravityVariance"_hs)
			.data<&ParticleEmitter::Continuous>("Continuous"_hs)
			.data<&ParticleEmitter::EmissionRate>("EmissionRate"_hs)
			.data<&ParticleEmitter::PreWarm>("PreWarm"_hs)
			.data<&ParticleEmitter::PreWarmSeconds>("PreWarmSeconds"_hs)
			.data<&ParticleEmitter::SetSeed, &ParticleEmitter::GetSeed>("Seed"_hs)
			.func<&ParticleEmitter::GenerateRandomSeed>("GenerateRandomSeed"_hs)
			.func<&ParticleEmitter::Emit>("Emit"_hs)
			.data<&ParticleEmitter::SetCount, &ParticleEmitter::GetCount>("GetCount"_hs)
			.data<&ParticleEmitter::Particles>("Particles"_hs)
			.type("ParticleEmitter"_hs);

		entt::meta<Particle>()
			.data<&Particle::Active>("Active"_hs)
			.data<&Particle::Position>("Position"_hs)
			.data<&Particle::Life>("Life"_hs)
			.data<&Particle::TotalLife>("TotalLife"_hs)
			.data<&Particle::Speed>("Speed"_hs)
			.data<&Particle::Angle>("Angle"_hs)
			.type("Particle"_hs);
	}

}