#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec4.h";
#include <imgui.h>
#include "Chroma/Renderer/Texture.h"
#include "Chroma/Assets/Sprite.h"

namespace Chroma
{
	struct SpriteRenderer : Component
	{

		Math::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::vec3 Offset { 0.0f, 0.0f, 0.0f };
		Math::vec3 Scale { 1.0f, 1.0f, 1.0f };
		std::string Layer = "Default";
		bool PlayOnStart = true;
		bool Playing = true;

		std::string Sprite;


		int CurrentFrame = 0;
		

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

		Ref<Chroma::Sprite> GetSprite();

	private:
		Ref<Chroma::Sprite> _sprite;

	};
}