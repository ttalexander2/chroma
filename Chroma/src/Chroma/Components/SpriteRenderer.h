#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Vec4.h";
#include "Chroma/Renderer/Texture.h"
#include "Chroma/Assets/Sprite.h"
#include "Chroma/Scene/World.h"

namespace Polychrome
{
	class ComponentWidgets;
}


namespace Chroma
{
	class SpriteRendererSystem;

	/// @brief Component to render a sprite. Supports both animated and non-animated.
	/// @see Sprite
	/// @see Aseprite
	struct SpriteRenderer : Component
	{
		CHROMA_COMPONENT(SpriteRenderer, Component);

		friend class Polychrome::ComponentWidgets;

		enum class SpriteOrigin
		{
			Center = 0,
			Left = 1,
			Right = 2,
			Top = 3,
			Bottom = 4,
			TopLeft = 5,
			TopRight = 6,
			BottomLeft = 7,
			BottomRight = 8,
			Custom = 9,

			Default = Center
		};

		/// @brief Color to render the sprite. Default is White.
		Math::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
		/// @brief Offset to render the sprite.
		Math::vec2 Offset { 0.0f, 0.0f};

		float SortingPoint = 0;
		/// @brief Whether to play the sprite on start.
		bool PlayOnStart = true;
		/// @brief Whether the sprite is playing.
		bool Playing = true;
		/// @brief Whether the sprite loops.
		bool Loop = true;
		/// @brief Speed of the sprite.
		float SpeedMultiplier = 1.0f;

		/// @brief Gets the sprite ID/Path
		/// @return Relative path/name of the sprite.
		const std::string& GetSpriteID() const { return SpriteID; }
		/// @brief Set the sprite to render.
		/// 
		/// Sprite must already be loaded.
		/// @see AssetManager to load sprite.
		/// @param spriteID Name/Path of the sprite.
		void SetSprite(const std::string& spriteID);

		/// @brief Set the animation of the sprite.
		/// @param animation Number of the animation.
		void SetAnimation(unsigned int animation);

		/// @brief Set the animation of the sprite.
		/// @param animation_name Name of the animation.
		void SetAnimation(const std::string& animation_name);

		/// @brief Get the number of the current animation.
		/// @return 
		unsigned int GetAnimation();

		/// @brief Get the name of the animation given the number.
		/// @param animation animation number.
		/// @return Name of the animation.
		std::string GetAnimationName(unsigned int animation);

		/// @brief Get the sprite's current frame.
		/// @return Current frame number.
		int GetCurrentFrame();

		/// @brief Set the sprite's current frame.
		/// @param Frame number to set.
		void SetCurrentFrame(unsigned int);

		void SetSpriteOrigin(SpriteOrigin origin);
		void SetSpriteOrigin(const Math::vec2& custom_position);

		SpriteOrigin GetSpriteOrigin();
		const Math::vec2& GetSpriteOriginVector();

		/// @brief Restart the animation.
		/// 
		/// WARNING: Not Implemented.
		void RestartAnimation();


		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	private:
		unsigned int CurrentFrame = 0;
		unsigned int Animation = 0;

		/// @brief Origin of the sprite
		SpriteOrigin Origin = SpriteOrigin::Default;
		Math::vec2 OriginValue = { 0, 0 };


		float time_till_next_frame = 0;
		bool looping_forward = 0;
		std::string SpriteID;

		friend class SpriteRendererSystem;

	};
}
