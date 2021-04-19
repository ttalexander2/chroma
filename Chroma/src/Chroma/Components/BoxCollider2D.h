#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/vec2.h"

namespace Chroma
{
	struct BoxCollider2D : Component
	{
		Math::vec2 Bounds{ 1.0f, 1.0f };
		Math::vec2 Offset{ 0.0f, 0.0f };

		BoxCollider2D() = default;
		BoxCollider2D(const BoxCollider2D&) = default;
		BoxCollider2D(const Math::vec2& bounds, const Math::vec2& offset)
			: Bounds(bounds), Offset(offset)
		{
		}

		const std::string Name() const override
		{
			return "Box Collider 2D";
		}
	};
}

