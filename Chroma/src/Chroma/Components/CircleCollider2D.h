#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec2.h"

namespace Chroma
{
	struct CircleCollider2D : Component
	{
		float Radius = 1.0f;
		Math::vec2 Offset{ 0.0f, 0.0f };

		const std::string Name() const override
		{
			return "Circle Collider 2D";
		}
	};
}

