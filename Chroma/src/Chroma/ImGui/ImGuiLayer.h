#pragma once

#include "Chroma/Layer.h"
#include "Chroma/Events/Event.h"
#include "Chroma/Events/MouseEvent.h"
#include "Chroma/Events/KeyEvent.h"
#include "Chroma/Events/ApplicationEvent.h"

namespace Chroma
{
	class CHROMA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
										
	private:							
		float m_Time;					
	};
}


