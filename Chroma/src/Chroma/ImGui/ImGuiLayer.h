#pragma once

#include "Chroma/Events/Event.h"
#include "Chroma/Events/MouseEvent.h"
#include "Chroma/Events/KeyEvent.h"
#include "Chroma/Events/ApplicationEvent.h"

namespace Chroma
{
	class CHROMA_API ImGuiLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnEvent(Event& e);

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
										
	private:			
		bool m_BlockEvents = true;
		float m_Time;					
	};
}


