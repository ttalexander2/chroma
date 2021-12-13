#pragma once

#include "Chroma/Scene/Component.h"
#include <string>

namespace Chroma
{
	/// @brief Tag component. Provides names for entities.
	struct Tag : public Component
	{
		CHROMA_COMPONENT(Tag, Component);

		/// @brief Entity Name.
		std::string EntityName = "New Entity";

		/// @brief Editor Order
		unsigned int Ordering = 0;


		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

		const inline void SetEnabled(bool enabled) override { m_Enabled = true; }

	};
}
