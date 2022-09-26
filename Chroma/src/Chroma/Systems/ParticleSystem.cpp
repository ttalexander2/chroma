#include "chromapch.h"
#include "ParticleSystem.h"
#include "Chroma/Components/ParticleEmitter.h"
#include "Chroma/Renderer/Renderer2D.h"
#include <glm/gtx/compatibility.hpp>


namespace Chroma
{
	void ParticleSystem::Init()
	{
		for (auto &e : m_Scene->Registry.view<ParticleEmitter>())
		{
			auto &emitter = m_Scene->GetComponent<ParticleEmitter>(e);
			auto &position = m_Scene->GetTransformAbsolutePosition(e);
			emitter.engine.seed(emitter._seed);
			for (Particle &p : emitter.Particles)
				p.Active = false;

			if (emitter.PreWarm)
			{
				float delta = 1.f / 60.f;
				for (int i = 0; i < static_cast<int>(glm::round(60 * emitter.PreWarmSeconds)); i++)
				{
					UpdateEmitter(emitter, position, delta);
				}
			}
		}
	}

	void ParticleSystem::Update(Time delta)
	{
		for (auto &e : m_Scene->Registry.view<ParticleEmitter>())
		{
			auto &emitter = m_Scene->GetComponent<ParticleEmitter>(e);
			if (!emitter.IsEnabled())
				continue;

			auto &position = m_Scene->GetTransformAbsolutePosition(e);
			UpdateEmitter(emitter, position, delta);
		}
	}


	void ParticleSystem::UpdateEmitter(ParticleEmitter &emitter, glm::vec2 position, Time delta)
	{
		if (emitter.Continuous)
		{
			emitter.timeSinceLastEmit += delta;
			if (emitter.timeSinceLastEmit >= 1 / emitter.EmissionRate)
			{
				emitter.Emit(position + glm::vec2((emitter.random() - 0.5f) * 2 * emitter.PositionVariance.x, (emitter.random() - 0.5f) * 2 * emitter.PositionVariance.y));
				emitter.timeSinceLastEmit = 0;
			}
		}

		for (Particle &p : emitter.Particles)
		{
			if (!p.Active)
				continue;

			p.Life -= static_cast<float>(delta);

			if (p.Life <= 0)
				p.Active = false;

			p.Position += ((glm::vec2(glm::cos(glm::radians(p.Angle)), glm::sin(glm::radians(p.Angle))) + glm::vec2(0, -emitter.Gravity)) * p.Speed + ((emitter.random() - 0.5f) * 2 * emitter.GravityVariance)) * static_cast<float>(delta);
		}
	}

	void ParticleSystem::Draw(Time delta)
	{
		for (auto &e : m_Scene->Registry.view<ParticleEmitter>())
		{
			auto &emitter = m_Scene->GetComponent<ParticleEmitter>(e);
			if (!emitter.IsEnabled())
				continue;
			Math::vec2 pos = m_Scene->GetTransformAbsolutePosition(e);

			for (Particle &p : emitter.Particles)
			{
				if (!p.Active)
					continue;

				Math::vec4 color = lerp(emitter.EndColor, emitter.StartColor, p.Life / p.TotalLife);
				Renderer2D::DrawQuad(p.Position, { 1, 1 }, color);
			}
		}
	}
}
