#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec4.h";
#include <imgui.h>

namespace Chroma
{
	struct SpriteRenderer : Component
	{
		Math::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Math::vec3 Offset{ 0.0f, 0.0f, 0.0f };
		std::string Layer = "Default";


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

		void DrawImGui() override;

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	};
}
