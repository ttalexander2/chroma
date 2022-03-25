#include "chromapch.h"
#include "Animator.h"

namespace Chroma
{

	void Animator::Serialize(YAML::Emitter& out)
	{
	}

	void Animator::Deserialize(YAML::Node& node)
	{
	}

	void Animator::CreateReflectionModel()
	{
		entt::meta<Animator>()
			.type("Animator"_hs);
	}
}


