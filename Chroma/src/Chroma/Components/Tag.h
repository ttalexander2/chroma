#pragma once

#include "Chroma/Scene/Component.h"
#include <string>

namespace Chroma
{
	struct Tag : Component
	{
		std::string EntityName = "ENTITY";

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

		void Serialize(YAML::Emitter& out) override
		{
			out << YAML::Key << "EntityName";
			out << YAML::Value << EntityName;
		}

		void Deserialize(YAML::Node& node) override
		{
			auto val = node["EntityName"];
			if (val)
			{
				EntityName = val.as<std::string>();
			}
		}

	private:

		const bool IsTag() const override
		{
			return true;
		}

	};
}
