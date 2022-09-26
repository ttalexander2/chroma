#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Animation/Animation.h"

namespace Chroma
{
	class AnimationSystem;

	struct AnimationPlayer : public Component
	{
		CHROMA_COMPONENT(AnimationPlayer, Component);

		bool PlayOnStart = true;

		void Start();
		void Restart();
		void Pause();
		void Stop();

		void SetAnimation(Animation *animation);

		bool IsPlaying() { return m_Playing; }

	private:
		bool m_Playing = false;
		float m_Time = 0.f;
		Animation *m_Current = nullptr;
		bool m_Reverse = false;
		std::unordered_map<Animation::Track *, std::multiset<Animation::Keyframe, Animation::TimeSort>::iterator> m_Keyframes;


		friend class AnimationSystem;
	};
}
