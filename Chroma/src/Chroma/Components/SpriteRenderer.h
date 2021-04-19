#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec4.h";

namespace Chroma
{
	struct SpriteRenderer : Component
	{
		Math::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		unsigned int Layer = 0;

		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(const Math::vec4 color)
			: Color(color)
		{
		}

		const std::string Name() const override
		{
			return "Sprite Renderer";
		}
	};
}
