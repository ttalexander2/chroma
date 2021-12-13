#pragma once

#include "Chroma/Scene/Component.h"


namespace Chroma
{
	struct Relationship : public Component
	{
		CHROMA_COMPONENT(Relationship, Component);

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