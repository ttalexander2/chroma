#pragma once
#include <vector>
#include <set>
#include <string>
#include "Chroma/Math/Vec2.h"
#include "Chroma/Assets/Asset.h"

#include <entt.hpp>


namespace Chroma
{

	struct Animation : public Asset
	{
		CHROMA_ASSET(Animation, Asset);

		enum class LoopType
		{
			none = 0,
			loop = 1,
			pingPong = 2,
		};

		enum class UpdateType
		{
			continuous = 0,
			discrete = 1
		};


		struct Transition
		{
			Math::vec2 a = { 0, 0 };
			Math::vec2 b = { 1, 1 };
		};

		struct Keyframe
		{
			float time;
			Transition transition;
			Reflection::Any value;
		};

		struct TimeSort
		{
			bool operator ()(const Keyframe& lhs, const Keyframe& rhs) const
			{
				return lhs.time < rhs.time;
			}
		};

		struct Track
		{
			UpdateType update;
			std::multiset<Keyframe, TimeSort> keyframes;
			Chroma::EntityID entityID;
			std::string component;
			std::string property;
		};

		float length;
		LoopType loop_type;
		std::vector<Track> tracks;
	};

}