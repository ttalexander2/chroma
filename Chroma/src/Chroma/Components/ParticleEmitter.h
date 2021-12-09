#pragma once

#include "Chroma/Scene/Component.h"
#include <Chroma/Renderer/Texture.h>
#include <random>

namespace Chroma
{

	class ParticleSystem;

	struct Particle
	{
		bool Active = false;
		glm::vec2 Position;
		float Life;
		float TotalLife;
		float Speed;
		float Angle;
	};


	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter() = default;
		ParticleEmitter(const ParticleEmitter&) = default;

		const std::string Name() const override
		{
			return StaticName();
		}

		const static std::string StaticName()
		{
			return "Particle Emitter";
		}

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;

		glm::vec2 PositionVariance = { 0, 0 };

		//Color
		glm::vec4 StartColor = {1.f, 0.f, 0.f, 1.f};
		glm::vec4 EndColor = {0.f, 0.f, 1.f, 1.f};

		//Life
		float Life = 2.f;
		float LifeVariance =  0.5f;
		
		//Angle
		float Angle = 0.f;
		float AngleVariance = 360.f;

		//Speed
		float Speed = 50.f;
		float SpeedVariance = 15.f;

		//Gravity
		float Gravity = 0.f;
		float GravityVariance = 0.f;

		//Settings
		bool Continuous = true;
		float EmissionRate = 5.f;
		bool PreWarm = true;
		float PreWarmSeconds = 2.f;
		

		const long long GetSeed() { return _seed; }
		void SetSeed(long long seed) { _seed = seed; engine.seed(_seed); }

		void GenerateRandomSeed();
		void Emit(const Math::vec2& position);

		constexpr size_t GetCount() { return Particles.size(); }
		void SetCount(size_t size) { Particles.resize(size); if (pos > size) pos = size - 1; }

		std::vector<Particle> Particles = std::vector<Particle>(200);

	private:
		
		size_t pos = 0;

		double timeSinceLastEmit = 0;

		long long _seed;

		std::default_random_engine engine;
		std::uniform_real_distribution<float> dist{ 0, 1 };

		float random();

		static std::random_device sRd;
		static std::default_random_engine sEngine;
		static std::uniform_int_distribution<long long> sDist;
		

		friend class ParticleSystem;

	};
}