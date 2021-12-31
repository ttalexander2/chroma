#pragma once

#include "Chroma/Scene/Component.h"
#include <bitset>

namespace Chroma
{
	struct Collider : public Component
	{
		CHROMA_ABSTRACT_COMPONENT(Collider, Component);

	private:

		unsigned long m_CollisionMask;
	};
}