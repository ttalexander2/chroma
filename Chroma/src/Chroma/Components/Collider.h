#pragma once

#include "Chroma/Scene/Component.h"
#include <bitset>

namespace Chroma
{
	struct Collider : public Component
	{
		CHROMA_ABSTRACT_COMPONENT(Collider, Component);

		unsigned long m_Layer = 1U;
		unsigned long m_Mask = 1U;
	};
}