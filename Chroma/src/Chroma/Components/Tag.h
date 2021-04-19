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

	private:

		const bool IsTag() const override
		{
			return true;
		}

	};
}
