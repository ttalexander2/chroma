#pragma once

#include "Chroma/Scene/Component.h"
#include <string>

namespace Chroma
{
	struct Tag : Component
	{
		std::string EntityName = "New Entity";

		Tag() = default;
		Tag(const Tag&) = default;
		Tag(const std::string& name)
			: EntityName(name)
		{
		}

		const std::string Name() const override
		{
			return "Tag";
		}

		const bool AllowMultiple() const override { return false; };

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	private:

		const bool IsTag() const override
		{
			return true;
		}

	};
}
