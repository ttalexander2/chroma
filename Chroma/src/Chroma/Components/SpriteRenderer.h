#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec4.h";
#include <imgui.h>
#include "Chroma/Renderer/Texture.h"
#include "Chroma/Assets/Sprite.h"

namespace Chroma
{
	class SpriteRendererSystem;

	struct SpriteRenderer : Component
	{
		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(const Math::vec4 color)
			: Color(color)
		{
		}


		Math::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::vec3 Offset { 0.0f, 0.0f, 0.0f };
		std::string Layer = "Default";
		bool PlayOnStart = true;
		bool Playing = true;
		bool Loop = true;
		float SpeedMultiplier = 1.0f;

		const std::string GetSpriteID() const { return SpriteID; }
		void SetSprite(const std::string& spriteID);
		void SetAnimation(unsigned int animation);
		void SetAnimation(const std::string& animation_name);
		int GetAnimation();
		std::string GetAnimationName(unsigned int animation);
		int GetCurrentFrame();
		void SetCurrentFrame(unsigned int);
		void RestartAnimation();
		void RestartAnimation(unsigned int animation);
		void RestartAnimation(const std::string& animation);


		const std::string Name() const override
		{
			return "Sprite Renderer";
		}

		void DrawImGui() override;

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	private:
		unsigned int CurrentFrame = 0;
		unsigned int Animation = 0;


		float time_till_next_frame = 0;
		bool looping_forward = 0;
		std::string SpriteID;

		friend class SpriteRendererSystem;

	};
}
