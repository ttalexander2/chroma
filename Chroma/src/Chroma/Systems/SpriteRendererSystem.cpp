#include "chromapch.h"
#include "SpriteRendererSystem.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Scene/EntityRef.h"
#include "Chroma/Components/Transform.h"
#include <Chroma/Renderer/Renderer2D.h>

namespace Chroma
{
	void SpriteRendererSystem::Init()
	{
		for (EntityRef e : m_Scene->View<SpriteRenderer>())
		{
			ComponentRef<SpriteRenderer> spriteRenderer = e.GetComponent<SpriteRenderer>();
			//load sprite
		}
	}

	void SpriteRendererSystem::Draw(Time delta)
	{
		for (EntityRef e : m_Scene->View<SpriteRenderer, Transform>())
		{
			ComponentRef<SpriteRenderer> spriteRenderer = e.GetComponent<SpriteRenderer>();
			ComponentRef<Transform> transform = e.GetComponent<Transform>();
			Chroma::Renderer2D::DrawQuad(transform->Position, transform->Scale, spriteRenderer->Color, transform->Rotation.x);
		}
	}
}


