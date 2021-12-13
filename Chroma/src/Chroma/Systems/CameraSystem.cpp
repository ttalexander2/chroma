#include "chromapch.h"


#include "CameraSystem.h"
#include "Chroma/Components/Transform.h"


namespace Chroma
{
	void CameraSystem::LateInit()
	{
		auto view = m_Scene->Registry.view<Camera, Transform>();
		for (EntityID e : view)
		{
			auto& cam = view.get<Camera>(e);
			auto& transform = view.get<Transform>(e);
			cam.SetPosition(transform.Position);
			if (cam.dirty)
			{
				cam.RecalculateViewMatrix();
				cam.dirty = false;
			}
		}
	}

	void CameraSystem::LateUpdate(Time time)
	{
		auto view = m_Scene->Registry.view<Camera, Transform>();
		for (EntityID e : view)
		{
			auto& cam = view.get<Camera>(e);

			if (!cam.IsEnabled())
				continue;

			auto& transform = view.get<Transform>(e);
			cam.SetPosition(transform.Position);
			if (cam.dirty)
			{
				cam.RecalculateViewMatrix();
				cam.dirty = false;
			}
		}
	}
}

