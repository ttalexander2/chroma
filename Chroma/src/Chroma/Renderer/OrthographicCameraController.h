#pragma once

#include "Chroma/Renderer/OrthographicCamera.h"
#include "Chroma/Core/Time.h"

#include "Chroma/Events/ApplicationEvent.h"
#include "Chroma/Events/MouseEvent.h"

namespace Chroma
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspect_ratio, bool rotation = false);

		void OnEvent(Event& e);
		void OnUpdate(Time ts);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() const { return m_ZoomLevel; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f;
		float m_CameraRotationSpeed = 30.0f;
	};
}