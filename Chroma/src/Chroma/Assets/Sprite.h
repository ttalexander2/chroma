#pragma once
#include <Chroma/Renderer/Texture.h>
#include "Chroma/Assets/Asset.h"

namespace Chroma
{
	struct Sprite: public Asset
	{

		Sprite(std::string& path) : Path(path) 
		{
		}

		enum class LoopDirection
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		struct Frame
		{
			unsigned int Durration = 100;
			Ref<Texture2D> Texture;
		};

		struct Animation
		{
			const std::string Tag;
			const bool Loop = true;
			const LoopDirection Direction = LoopDirection::Forward;
			const int Start;
			const int End;
		};

		std::string Path;
		std::vector<Animation> Animations;
		std::vector<Frame> Frames;

	};
}