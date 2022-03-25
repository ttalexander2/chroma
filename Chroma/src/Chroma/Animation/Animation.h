#pragma once
#include <vector>
#include <any>
#include <string>
#include "Chroma/Math/Vec2.h"


namespace Chroma
{

	enum class LoopType
	{
		None = 0,
		Loop = 1,
		PingPong = 2,
	};

	enum class UpdateType
	{
		Continuous = 0,
		Discrete = 1
	};


	struct Transition
	{
		Math::vec2 a = { 0, 0 };
		Math::vec2 b = { 1, 1 };
	};

	struct IKeyframe
	{
		float m_Time;
		Transition m_Transition;
	};

	template <typename T>
	struct Keyframe : public IKeyframe
	{
		T m_Data;
	};

	struct Track
	{
		std::string m_Name;
		std::string m_ComponentName;
		std::string m_DataName;
		UpdateType m_Update;

		std::vector<IKeyframe*> m_Keyframes;
	};

	struct Animation
	{
		float m_Length;
		LoopType m_LoopType;
		std::vector<Track> m_Tracks;
	};

}