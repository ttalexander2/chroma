#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Scene/ECS.h"


namespace Chroma
{
	struct Relationship : public Component
	{
		std::vector<EntityID> Children;
		EntityID Parent{ ENTITY_NULL };

		const bool HasChildren() const { return Children.size() > 0; };
		const bool IsChild() const { return Parent != ENTITY_NULL; };

		const bool HasChild(EntityID child) const { return std::find(Children.begin(), Children.end(), child) != Children.end(); }


		const std::string Name() const override { return StaticName(); }

		const static std::string StaticName()
		{
			return "Relationship";
		}

		const bool EditorVisible() const override { return false; }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;

	};
}