#include "chromapch.h"
#include "Layer.h"

namespace Chroma
{
	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName), m_Enabled(true)
	{
	}

	Layer::~Layer()
	{

	}

	void Layer::Enable()
	{
		m_Enabled = true;
	}

	void Layer::Disable()
	{
		m_Enabled = false;
	}
}
