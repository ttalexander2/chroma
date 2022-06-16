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

		void SetAnimation(Animation* animation);

		inline bool IsPlaying() { return m_Playing; }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;

	private:
		bool m_Playing = false;
		float m_Time = 0.f;
		Animation* m_Current = nullptr;
		bool m_Reverse = false;
		std::unordered_map<Chroma::Animation::Track*, std::multiset<Chroma::Animation::Keyframe, Chroma::Animation::TimeSort>::iterator> m_Keyframes;


		friend class AnimationSystem;
	};
}