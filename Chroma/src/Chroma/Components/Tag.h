#pragma once

#include "Chroma/Scene/Component.h"
#include <string>

namespace Chroma
{
	/// @brief Tag component. Provides names for entities.
	struct Tag : Component
	{
		/// @brief Entity Name.
		std::string EntityName = "New Entity";

		/// @brief Editor Order
		unsigned int Ordering = 0;

		/// @brief Constructs an empty Tag component.
		Tag() = default;
		/// @brief Constructs a Tag component from an existing Tag.
		/// @param  Tag to copy.
		Tag(const Tag&) = default;
		/// @brief Constructs a new Tag.
		/// @param name Entity name for tag.
		Tag(const std::string& name)
			: EntityName(name)
		{
		}

		const std::string Name() const override { return StaticName(); }
		const static std::string StaticName()
		{
			return "Tag";
		}


		const bool EditorVisible() const override { return false; }

		const bool AllowMultiple() const override { return false; };

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	};
}
