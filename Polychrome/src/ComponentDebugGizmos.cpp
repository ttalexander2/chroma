#include "ComponentDebugGizmos.h"
#include "EditorApp.h"
#include "Hierarchy.h"
#include "Fonts/IconsForkAwesome.h"
#include "imgui_internal.h"
#include "Viewport.h"
#include <math.h>
#include <Chroma/Components/BoxCollider.h>

namespace Polychrome
{
	Math::uvec2 ComponentDebugGizmos::GridSize = {16.f, 16.f};
	Math::uvec2 ComponentDebugGizmos::SnapSize = { 16.f, 16.f };
	float ComponentDebugGizmos::GridOpacity = .5f;

	bool ComponentDebugGizmos::DrawAllGizmos = true;
	bool ComponentDebugGizmos::DrawCameraGizmos = true;
	bool ComponentDebugGizmos::DrawBoxCollider = false;

	bool ComponentDebugGizmos::DrawAllEntities = false;

	void ComponentDebugGizmos::DrawGizmos()
	{
		if (!DrawAllGizmos)
			return;
		//CAMERA
		if (DrawCameraGizmos)
		{
			auto camEntity = EditorApp::CurrentScene->GetPrimaryCameraEntity();
			auto& camPos = EditorApp::CurrentScene->GetTransformAbsolutePosition(camEntity);
			Chroma::Renderer2D::DrawRect(camPos, EditorApp::CurrentScene->GetPrimaryCamera().GetSize(), 1.f / EditorApp::Camera.GetZoom(), { 0.2f, 0.3f , 0.8f , 1.f });
		}



		//COMPONENTS
		if (!DrawAllEntities && Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
		{
			DrawBoxColliderGizmos(Hierarchy::SelectedEntity);
		}
		else if (DrawAllEntities)
		{
			for (auto entity : EditorApp::CurrentScene->Registry.view<Chroma::BoxCollider>())
			{
				DrawBoxColliderGizmos(entity);
			}
		}
	}

	void ComponentDebugGizmos::DrawBoxColliderGizmos(Chroma::EntityID entity)
	{
		if (DrawBoxCollider)
		{
			auto& collider = EditorApp::CurrentScene->GetComponent<Chroma::BoxCollider>(entity);
			const Math::vec2 absolutePos = EditorApp::CurrentScene->GetTransformAbsolutePosition(entity);
			Chroma::Renderer2D::DrawRect(absolutePos + collider.Offset, collider.Bounds, 1.f / EditorApp::Camera.GetZoom(), { 0.2f, 0.9f , 0.3f , 1.f });
		}
	}

	void ComponentDebugGizmos::DrawIcons()
	{
		if (!DrawAllGizmos)
			return;

		if (DrawCameraGizmos)
		{
			float y = ImGui::GetCursorPosY();
			//CAMERA
			auto camEntity = EditorApp::CurrentScene->GetPrimaryCameraEntity();
			auto& camPos = EditorApp::CurrentScene->GetTransformAbsolutePosition(camEntity);
			auto& camSize = EditorApp::CurrentScene->GetPrimaryCamera().GetSize();
			Math::vec2 pos = { camPos.x - 10.f / EditorApp::Camera.GetZoom() , camPos.y + 10.f / EditorApp::Camera.GetZoom() + camSize.y / 2.f };
			Math::vec2 screen = Viewport::WorldToViewportPosition(pos);
			if (y - screen.y < ImGui::GetWindowContentRegionMax().y - 15.f)
			{
				ImGui::SetCursorPos({ screen.x, y - screen.y });
				ImGui::Text(ICON_FK_CAMERA);
			}
		}



		//COMPONENTS
	}

	void ComponentDebugGizmos::DrawGrid()
	{

		const Math::uvec2& size = EditorApp::Camera.GetSize();
		const Math::vec2& pos = EditorApp::Camera.GetPosition();
		const float zoom = EditorApp::Camera.GetZoom();
		const float zoomMultiplier = Math::pow(10.f, Math::floor(log10f(zoom)));
		float opacity = (zoom - zoomMultiplier) / zoom * GridOpacity;
		const float line_width = 1.f / EditorApp::Camera.GetZoom();
		float x_mult = GridSize.x * 1 / zoomMultiplier;
		float y_mult = GridSize.y * 1 / zoomMultiplier;

		
		
		//Draw Horizontal
		float xPos = 0;
		float yPos = 0;
		float xPosInitial = glm::round(pos.x / x_mult) * x_mult;
		float yPosInitial = glm::round(pos.y / y_mult) * y_mult;
		int xFactor = (int)glm::round(pos.x / x_mult) % 10;
		int yFactor = (int)glm::round(pos.y / y_mult) % 10;


		const float hSize = size.x / zoom;
		const float vSize = size.y / zoom;

		int split_x = 10;
		int split_y = 10;
		if (zoomMultiplier >= 10)
		{
			x_mult = 1;
			y_mult = 1;
			split_x = glm::round(GridSize.x);
			split_y = glm::round(GridSize.y);
			xFactor = (int)glm::round(pos.x / x_mult) % split_x;
			yFactor = (int)glm::round(pos.y / y_mult) % split_y;
			xPosInitial = glm::round(pos.x / x_mult) * x_mult;
			yPosInitial = glm::round(pos.y / y_mult) * y_mult;
			if (zoomMultiplier >= 100)
				opacity = GridOpacity;
		}
			

		//CHROMA_CORE_INFO("Mult: {}, {}", x_mult, y_mult);

		bool first = true;

		int i = xFactor;
		int j = xFactor;
		while (xPos < hSize / 2.f)
		{
			if (j % split_x == 0)
			{
				Chroma::Renderer2D::DrawLine({ xPosInitial - xPos, pos.y + vSize / 2.f }, { xPosInitial - xPos, pos.y - vSize / 2.f }, line_width, { 1.f, 1.f, 1.f, GridOpacity });
			}
			else
			{
				Chroma::Renderer2D::DrawLine({ xPosInitial - xPos, pos.y + vSize / 2.f }, { xPosInitial - xPos, pos.y- vSize / 2.f }, line_width, { 1.f, 1.f, 1.f, opacity });
			}

			if (!first)
			{
				if (i % split_x == 0)
				{
					Chroma::Renderer2D::DrawLine({ xPosInitial + xPos, pos.y + vSize / 2.f }, { xPosInitial + xPos, pos.y - vSize / 2.f }, line_width, { 1.f, 1.f, 1.f, GridOpacity });
				}
				else
				{
					Chroma::Renderer2D::DrawLine({ xPosInitial + xPos, pos.y + vSize / 2.f }, { xPosInitial + xPos, pos.y - vSize / 2.f }, line_width, { 1.f, 1.f, 1.f, opacity });
				}
			}


			xPos += x_mult;
			i++;
			j--;
			first = false;
		}

		i = yFactor;
		j = yFactor;
		first = true;
		while (yPos < vSize / 2.f)
		{
			if (j % split_y == 0)
			{

				Chroma::Renderer2D::DrawLine({ pos.x + hSize / 2.f, yPosInitial - yPos }, { pos.x - hSize / 2.f, yPosInitial - yPos }, line_width, { 1.f, 1.f, 1.f, GridOpacity });
			}
			else
			{

				Chroma::Renderer2D::DrawLine({ pos.x + hSize / 2.f, yPosInitial - yPos }, { pos.x - hSize / 2.f, yPosInitial - yPos }, line_width, { 1.f, 1.f, 1.f, opacity });
			}
			if (!first)
			{
				if (i % split_y == 0)
				{
					Chroma::Renderer2D::DrawLine({ pos.x + hSize / 2.f, yPosInitial + yPos }, { pos.x - hSize / 2.f, yPosInitial + yPos }, line_width, { 1.f, 1.f, 1.f, GridOpacity });
				}
				else
				{
					Chroma::Renderer2D::DrawLine({ pos.x + hSize / 2.f, yPosInitial + yPos }, { pos.x - hSize / 2.f, yPosInitial + yPos }, line_width, { 1.f, 1.f, 1.f, opacity });
				}
			}

			yPos += y_mult;
			i++;
			j--;
			first = false;
		}
	}
}