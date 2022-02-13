#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Utilities/GUID.h"
#include <unordered_set>


namespace Chroma
{

	struct EntityInfo : public Component
	{
		CHROMA_COMPONENT(EntityInfo, Component);

		std::string Name = "New Entity";
		unsigned int Layer = 0;
		unsigned int Ordering = 0;

		std::unordered_set<std::string> Tags;
		GUID ID = GUID::CreateGUID();



		std::vector<EntityID> Children;
		EntityID Parent{ ENTITY_NULL };

		const bool HasChildren() const { return Children.size() > 0; };
		const bool IsChild() const { return Parent != ENTITY_NULL; };

		const bool HasChild(EntityID child) const { return std::find(Children.begin(), Children.end(), child) != Children.end(); }

		const inline void SetEnabled(bool enabled) override { m_Enabled = true; }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;

	};

}