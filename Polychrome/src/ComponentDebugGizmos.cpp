#include "ComponentDebugGizmos.h"
#include "EditorApp.h"
#include "Hierarchy.h"
#include "Fonts/IconsForkAwesome.h"
#include "imgui_internal.h"
#include "Viewport.h"
#include <math.h>
#include <Chroma/Components/ParticleEmitter.h>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Components/Collider.h>
#include <imgui.h>

namespace Polychrome
{
	Math::uvec2 ComponentDebugGizmos::GridSize = {16.f, 16.f};
	Math::uvec2 ComponentDebugGizmos::SnapSize = { 16.f, 16.f };
	float ComponentDebugGizmos::GridOpacity = .5f;

	bool ComponentDebugGizmos::DrawAllGizmos = true;
	bool ComponentDebugGizmos::DrawCameraGizmos = true;
	bool ComponentDebugGizmos::DrawBoxCollider = false;
	bool ComponentDebugGizmos::DrawSpriteBoundries = false;
	bool ComponentDebugGizmos::DrawParticleEmtiters = true;

	bool ComponentDebugGizmos::DrawAllEntities = false;

	void ComponentDebugGizmos::DrawGizmos()
	{
		if (!DrawAllGizmos)
			return;
		//CAMERA
		if (DrawCameraGizmos)
		{
			auto camEntity = EditorApp::CurrentScene->GetPrimaryCameraEntity();
			if (EditorApp::CurrentScene->Registry.valid(camEntity))
			{
				auto& camPos = EditorApp::CurrentScene->GetTransformAbsolutePosition(camEntity);
				Chroma::Renderer2D::DrawRect(camPos, EditorApp::CurrentScene->GetPrimaryCamera().GetSize(), 1.f / EditorApp::Camera.GetZoom(), { 0.2f, 0.3f , 0.8f , 1.f });
			}

		}



		//COMPONENTS
		if (!DrawAllEntities && Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
		{
			DrawColliderGizmos(Hierarchy::SelectedEntity);
			DrawSpriteBoundryGizmos(Hierarchy::SelectedEntity);
		}
		else if (DrawAllEntities)
		{
			for (auto entity : EditorApp::CurrentScene->Registry.view<Chroma::Collider>())
			{
				DrawColliderGizmos(entity);
			}

			for (auto entity : EditorApp::CurrentScene->Registry.view<Chroma::SpriteRenderer>())
			{
				DrawSpriteBoundryGizmos(entity);
			}
		}
	}

	void ComponentDebugGizmos::DrawColliderGizmos(Chroma::EntityID entity)
	{
		if (DrawBoxCollider && EditorApp::CurrentScene->HasComponent<Chroma::Collider>(entity))
		{
			auto& collider = EditorApp::CurrentScene->GetComponent<Chroma::Collider>(entity);
			const Math::vec2 absolutePos = EditorApp::CurrentScene->GetTransformAbsolutePosition(entity);
		}
	}



	void ComponentDebugGizmos::DrawSpriteBoundryGizmos(Chroma::EntityID entity)
	{
		if (DrawSpriteBoundries && EditorApp::CurrentScene->HasComponent<Chroma::SpriteRenderer>(entity))
		{
			Chroma::Transform& transform = EditorApp::CurrentScene->Registry.get<Chroma::Transform>(entity);
			Chroma::SpriteRenderer& spriteRenderer = EditorApp::CurrentScene->Registry.get<Chroma::SpriteRenderer>(entity);

			const Math::vec2& origin = spriteRenderer.GetSpriteOriginVector();

			if (Chroma::AssetManager::Exists(spriteRenderer.GetSpriteID()))
			{
				Chroma::Ref<Chroma::Sprite> s = Chroma::AssetManager::Get<Chroma::Sprite>(spriteRenderer.GetSpriteID());
				int w = s->Frames[spriteRenderer.GetCurrentFrame()].Texture->GetWidth();
				int h = s->Frames[spriteRenderer.GetCurrentFrame()].Texture->GetHeight();
				if (!transform.IsChild())
				{
					Math::vec2 size = transform.Scale * Math::vec2((float)w, (float)h);
					Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };

					Math::vec2 center = transform.Position + spriteRenderer.Offset + originAdjustment;
					Math::vec2 sortPointPosition = {center.x, center.y + spriteRenderer.SortingPoint};

					Chroma::Renderer2D::DrawRect(transform.Position + spriteRenderer.Offset + originAdjustment, transform.Scale * Math::vec2((float)w, (float)h), 2.f / EditorApp::Camera.GetZoom(), { 0.95f, 0.0f , 0.5f , 1.f });
					Chroma::Renderer2D::DrawLine({ center.x - size.x / 2.f, center.y + spriteRenderer.SortingPoint }, { center.x + size.x / 2.f, center.y + spriteRenderer.SortingPoint }, 2.f / EditorApp::Camera.GetZoom(), { 0.95f, 0.0f , 0.5f , 1.f });
				}
				else
				{
					Math::vec2 pos = transform.Position;
					Math::vec2 scale = transform.Scale;
					Math::vec2 parentPos{ 0,0 };
					float parentRot = 0;
					float rotation = transform.Rotation;
					Chroma::EntityID parent = transform.Parent;
					while (parent != Chroma::ENTITY_NULL)
					{
						Chroma::Transform& parentTransform = EditorApp::CurrentScene->GetComponent<Chroma::Transform>(parent);
						parentPos += parentTransform.Position;
						scale *= parentTransform.Scale;
						parentRot += parentTransform.Rotation;
						parent = EditorApp::CurrentScene->GetComponent<Chroma::Transform>(parent).Parent;
					}

					Math::vec2 adjusted = { pos.x * Math::cos(parentRot) - pos.y * Math::sin(parentRot), pos.x * Math::sin(parentRot) + pos.y * Math::cos(parentRot) };
					//CHROMA_CORE_TRACE("Adjusted: [{}, {}]; ParentPos: [{}, {}]; ParentRot: {}", adjusted.x, adjusted.y, parentPos.x, parentPos.y, parentRot);
					Math::vec2 size = scale * Math::vec2((float)w, (float)h);
					Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };

					Math::vec2 center = parentPos + adjusted + spriteRenderer.Offset + originAdjustment;
					Math::vec2 sortPointPosition = { center.x, center.y + spriteRenderer.SortingPoint };


					Chroma::Renderer2D::DrawRect(parentPos + adjusted + spriteRenderer.Offset + originAdjustment, scale * Math::vec2((float)w, (float)h), 2.f / EditorApp::Camera.GetZoom(), { 0.95f, 0.0f , 0.5f , 1.f });
					Chroma::Renderer2D::DrawLine({ center.x - size.x / 2.f, center.y + spriteRenderer.SortingPoint }, { center.x + size.x / 2.f, center.y + spriteRenderer.SortingPoint }, 2.f / EditorApp::Camera.GetZoom(), { 0.95f, 0.0f , 0.5f , 1.f });
				}

			}

			
		}
	}

	void ComponentDebugGizmos::DrawIcons()
	{
		float y = ImGui::GetCursorPosY();

		if (DrawCameraGizmos)
		{
			//CAMERA
			auto camEntity = EditorApp::CurrentScene->GetPrimaryCameraEntity();
			if (EditorApp::CurrentScene->Registry.valid(camEntity))
			{
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

		}



		//COMPONENTS

		if (DrawParticleEmtiters)
		{
			if (!DrawAllEntities && Hierarchy::SelectedEntity != Chroma::ENTITY_NULL && EditorApp::CurrentScene->HasComponent<Chroma::ParticleEmitter>(Hierarchy::SelectedEntity))
			{
				Math::vec2 pos = EditorApp::CurrentScene->GetTransformAbsolutePosition(Hierarchy::SelectedEntity);
				Math::vec2 screen = Viewport::WorldToViewportPosition(pos);
				if (y - screen.y < ImGui::GetWindowContentRegionMax().y - 15.f)
				{
					ImGui::SetCursorPos({ screen.x - 6.f, y - screen.y - 6.f });
					ImGui::PushFont(EditorApp::SmallIcons);
					ImGui::Text(ICON_FK_MAGIC);
					ImGui::PopFont();
				}
			}
			else if (DrawAllEntities)
			{
				for (auto& e : EditorApp::CurrentScene->Registry.view<Chroma::ParticleEmitter>())
				{
					Math::vec2 pos = EditorApp::CurrentScene->GetTransformAbsolutePosition(e);
					Math::vec2 screen = Viewport::WorldToViewportPosition(pos);
					if (y - screen.y < ImGui::GetWindowContentRegionMax().y - 15.f)
					{
						ImGui::SetCursorPos({ screen.x - 6.f, y - screen.y - 6.f });
						ImGui::PushFont(EditorApp::SmallIcons);
						ImGui::Text(ICON_FK_MAGIC);
						ImGui::PopFont();
					}
				}
			}


		}
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