#pragma once

#include "Chroma/Core.h"
#include "Chroma/Events/Event.h"

namespace Chroma
{
	class CHROMA_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		void Enable();
		void Disable();
		bool IsEnabled() const { return m_Enabled; }

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		bool m_Enabled;
		std::string m_DebugName;
	};
}


