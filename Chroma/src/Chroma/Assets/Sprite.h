#pragma once
#include <Chroma/Renderer/Texture.h>
#include "Chroma/Assets/Asset.h"
#include <Chroma/Images/Aseprite.h>

namespace Chroma
{
	/// @brief Animated or Static Sprite.
	///
	/// Class for Game Sprite. This sprite can be static or animated.
	struct Sprite : Asset
	{
		CHROMA_ASSET(Sprite, Asset)

		/// @brief Enum for looping direction
		enum class LoopDirection
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		/// @brief Single animation frame.
		struct Frame
		{
			unsigned int Duration = 100;
			Ref<Texture2D> Texture;
		};

		/// @brief Sprite animation.
		struct Animation
		{
			std::string Tag;
			LoopDirection Direction = LoopDirection::Forward;
			unsigned int Start;
			unsigned int End;
			Color LabelColor;
		};

		/// @brief List of sprite animations.
		std::vector<Animation> Animations;

		/// @brief List of sprite frames.
		std::vector<Frame> Frames;

		const Math::vec2 GetSize() const { return m_Size; }

		/// @brief Returns true if the sprite is animated.
		bool Animated() { return Animations.size() > 0; }

	protected:
		bool Load() override;
		bool Unload() override;
		bool Reload() override;

	private:
		Math::vec2 m_Size = { 1.f, 1.f };
	};
}
