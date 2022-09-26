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

		const void SetEnabled(bool enabled) override { m_Enabled = true; }
	};
}
