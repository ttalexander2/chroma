#pragma once

#include "Chroma/Events/SystemEvent.h"
#include "Chroma/Events/MouseEvent.h"
#include "Chroma/Events/KeyEvent.h"
#include "Chroma/Events/ApplicationEvent.h"

namespace Chroma
{
	/// @brief Layer for rendering ImGui components.
	class ImGuiLayer
	{
	public:
		/// @brief Construct a new ImGuiLayer.
		ImGuiLayer();
		/// @brief Destroy the ImGuiLayer.
		~ImGuiLayer();

		/// @brief Function called on the attachment of the layer.
		virtual void OnAttach();
		/// @brief Function called on the detachment of the layer.
		virtual void OnDetach();

		/// @brief Called on Events. Allows ImGuiLayer to handle its own events.
		/// @param e Event to be handled.
		virtual void OnEvent(SystemEvent &e);

		/// @brief Begins the ImGui step for this frame.
		void Begin();
		/// @brief Ends the ImGui step for this frame.
		void End();

		/// @brief Tells the layer whether to block events.
		/// @param block Layer will block events from other layers if true.
		void BlockEvents(bool block) { m_BlockEvents = block; }

	private:
		bool m_BlockEvents = true;
		float m_Time;
	};
}
