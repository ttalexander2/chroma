#pragma once

#include "Chroma/Math/Math.h"

namespace Chroma
{
	class Physics
	{
	public:
		static const float GetScale() { return m_Scale; }
		static void SetScale(float scale) { m_Scale = Math::max(0.00000001f, scale); }

	private:
		static float m_Scale;
	};
}
