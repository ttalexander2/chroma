#pragma once
#include <Chroma/Renderer/Texture.h>
#include "Chroma/Assets/Asset.h"
#include <Chroma/Images/Aseprite.h>

namespace Chroma
{

	/// @brief Animated or Static Sprite.
	///
	/// Class for Game Sprite. This sprite can be static or animated.
	struct Sprite: public Asset
	{
		Sprite() = default;
		Sprite(const Sprite&) = default;
		Sprite(const std::string& path)
		{
			Path = path;
		}

		~Sprite()
		{
			Unload();
		}


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
			unsigned int Durration = 100;
			Ref<Texture2D> Texture;
		};

		/// @brief Sprite animation.
		struct Animation
		{
			std::string Tag;
			LoopDirection Direction = LoopDirection::Forward;
			int Start;
			int End;
			Chroma::Color LabelColor;
		};

		/// @brief List of sprite animations.
		std::vector<Animation> Animations;

		/// @brief List of sprite frames.
		std::vector<Frame> Frames;

		bool Load() override;
		bool Unload() override;
		bool Reload() override;

		/// @brief Returns true if the sprite is animated.
		bool Animated() { return Animations.size() > 0; }


	};
}