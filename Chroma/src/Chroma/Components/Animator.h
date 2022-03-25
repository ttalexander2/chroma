#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Animation/Animation.h"

namespace Chroma
{

	struct Animator : public Component
	{
		CHROMA_COMPONENT(Animator, Component);

		size_t Current = 0;
		std::vector<Animation> Animations;

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;
	};
}