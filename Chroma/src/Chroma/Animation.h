#pragma once
#include <cstdint>
#include <vector>

namespace Chroma
{
	struct AnimatedSprite
	{
		enum class LoopDirection
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		struct Frame
		{
			int Duration;
			uint32_t Texture;
		};

		bool Loop = true;
		LoopDirection Direction = LoopDirection::Forward;
		std::vector<Frame> Frames;

	};
}