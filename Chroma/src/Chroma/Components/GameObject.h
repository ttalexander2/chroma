#pragma once
#include "Chroma/Scene/Component.h"
#include "Chroma/Utilities/GUID.h"

namespace Chroma
{
	struct GameObject : public Component
	{
		GUID ID;
		std::string Name = "New Entity";
		unsigned int Ordering = 0;

		GUID Parent = GUID::Zero();
		std::vector<GUID> Children;

		const bool HasChildren() const { return Children.size() > 0; };
		const bool IsChild() const { return Parent != GUID::Zero(); };
		const bool HasChild(GUID child) const { return std::find(Children.begin(), Children.end(), child) != Children.end(); }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;
	};
}