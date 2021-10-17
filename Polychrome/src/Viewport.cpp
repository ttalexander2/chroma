#include "Viewport.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "Fonts/IconsForkAwesome.h"
#include <ImGuizmo.h>
#include "Hierarchy.h"
#include "EditorApp.h"
#include <Chroma/Components/Transform.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <Chroma/Math/Decompose.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Scripting/ScriptEngineRegistry.h>
#include <Chroma/Scripting/MonoScripting.h>
#include "ComponentDebugGizmos.h"

namespace Polychrome
{
	static glm::vec2 s_ViewportSize = glm::vec2(1270, 720);

	static bool shouldUpdate = true;
	static bool viewportFocused = false;
	static bool viewportHovered = false;
	static ImVec2 viewportMousePos = { 0,0 };
	static Chroma::Window* s_ViewportWindow;

	bool Viewport::Open = true;
	bool Viewport::ShouldDrawGrid = true;
	bool Viewport::SnapToGrid = true;

	static const float identityMatrix[16] =
	{ 
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f 
	};

	inline float snapf(float original, int numerator, int denominator)
	{
		return round(original * denominator / numerator) * numerator / denominator;
	}


	void Viewport::Draw(Chroma::Time time, Chroma::Ref<Chroma::Framebuffer> frame_buffer)
	{
		
		if (Open)
		{
			ImGui::Begin(ICON_FK_GAMEPAD " Viewport", &Open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			Chroma::Application::Get().GetImGuiLayer()->BlockEvents(!viewportFocused && !ImGui::IsWindowHovered());



			//float width, height;
			//// Case 1: Window is wider than 16:9
			//if (viewportPanelSize.x / viewportPanelSize.y > 16.0f / 9.0f)
			//{
			//	height = viewportPanelSize.y;
			//	width = viewportPanelSize.y * (16.0f / 9.0f);
			//}
			//// Case 1: Window is narrower than 16:9
			//else
			//{
			//	height = viewportPanelSize.x * (9.0f / 16.0f);
			//	width = viewportPanelSize.x;
			//}
			//height = glm::round(height);
			//width = glm::round(width);
			//
			//if ((s_ViewportSize.x != width || s_ViewportSize.y != height) && width > 0 && height > 0)
			//{
			//	frame_buffer->Resize((uint32_t)width, (uint32_t)height);
			//	s_ViewportSize = { width, height };
			//}


		
			//TOOLBAR

			static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::BOUNDS;

			ImVec4 oldButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);


			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));

			ImGui::Dummy({ 5, 0 });
			ImGui::SameLine();

			static std::string clone;

			// PLAY BUTTON
			if (EditorApp::SceneRunning && !EditorApp::ScenePaused)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)); //GRAY
			else
				ImGui::PushStyleColor(ImGuiCol_Text, { 0.1f, 0.8f, 0.2f, 1.0f }); // GREEN
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, EditorApp::SceneRunning && !EditorApp::ScenePaused);
			if (ImGui::Button(ICON_FK_PLAY))
			{
				if (!EditorApp::ScenePaused)
				{
					clone = EditorApp::CurrentScene->Serialize();
					EditorApp::CurrentScene->Load();
					EditorApp::CurrentScene->Init();
				}
				EditorApp::SceneRunning = true;
				EditorApp::ScenePaused = false;
			}
			ImGui::PopItemFlag();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			if (!EditorApp::SceneRunning || EditorApp::ScenePaused)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)); //GRAY
			else
				ImGui::PushStyleColor(ImGuiCol_Text, { 0.25f, 0.5f, 0.95f, 1.0f }); // BLUE
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !EditorApp::SceneRunning || EditorApp::ScenePaused);
			if (ImGui::Button(ICON_FK_PAUSE))  EditorApp::ScenePaused = true;
			ImGui::PopItemFlag();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			if (!EditorApp::SceneRunning)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)); //GRAY
			else
				ImGui::PushStyleColor(ImGuiCol_Text, { 0.95f, 0.25f, 0.25f, 1.0f }); // RED
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !EditorApp::SceneRunning);
			if (ImGui::Button(ICON_FK_STOP)) 
			{
				Chroma::Scene* out = new Chroma::Scene();
				if (Chroma::Scene::Deserialize(out, clone))
				{
					delete EditorApp::CurrentScene;
					clone.clear();
					EditorApp::CurrentScene = out;
					if (!EditorApp::CurrentScene->Registry.valid(Hierarchy::SelectedEntity))
						Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
					Chroma::ScriptEngineRegistry::RegisterAll();
					Chroma::MonoScripting::SetDeltaTime(0.f, 0.f);
					Chroma::MonoScripting::SetSceneContext(EditorApp::CurrentScene);
					auto view = EditorApp::CurrentScene->Registry.view<Chroma::CSharpScript>();
					for (Chroma::EntityID entity : view)
					{
						auto& script = view.get<Chroma::CSharpScript>(entity);
						auto entityObj = Chroma::Entity(entity, EditorApp::CurrentScene);
						Chroma::MonoScripting::InitScriptEntity(entityObj);
					}
				}
				else
				{
					delete out;
				}
				EditorApp::SceneRunning = false;
				EditorApp::ScenePaused = false;
					
			}
			ImGui::PopItemFlag();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			if (!EditorApp::SceneRunning)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)); //GRAY
			else
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text)); // RED
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !EditorApp::SceneRunning);
			if (ImGui::Button(ICON_FK_UNDO))
			{
				Chroma::Scene* out = new Chroma::Scene();
				if (Chroma::Scene::Deserialize(out, clone))
				{
					delete EditorApp::CurrentScene;
					clone.clear();
					EditorApp::CurrentScene = out;
					if (!EditorApp::CurrentScene->Registry.valid(Hierarchy::SelectedEntity))
						Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
					Chroma::ScriptEngineRegistry::RegisterAll();
					Chroma::MonoScripting::SetDeltaTime(0.f, 0.f);
					Chroma::MonoScripting::SetSceneContext(EditorApp::CurrentScene);
					auto view = EditorApp::CurrentScene->Registry.view<Chroma::CSharpScript>();
					for (Chroma::EntityID entity : view)
					{
						auto& script = view.get<Chroma::CSharpScript>(entity);
						auto entityObj = Chroma::Entity(entity, EditorApp::CurrentScene);
						Chroma::MonoScripting::InitScriptEntity(entityObj);
					}
				}
				else
				{
					delete out;
				}
				clone = EditorApp::CurrentScene->Serialize();
				EditorApp::CurrentScene->Load();
				EditorApp::CurrentScene->Init();

				EditorApp::SceneRunning = true;
				EditorApp::ScenePaused = false;
			}
			ImGui::PopItemFlag();
			ImGui::PopStyleColor();


			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

			if (operation == ImGuizmo::OPERATION::BOUNDS)
				ImGui::PushStyleColor(ImGuiCol_Button, oldButtonColor);
			if (ImGui::Button(ICON_FK_MOUSE_POINTER) && operation != ImGuizmo::OPERATION::BOUNDS)
				operation = ImGuizmo::OPERATION::BOUNDS;
			else if (operation == ImGuizmo::OPERATION::BOUNDS)
				ImGui::PopStyleColor();

			ImGui::SameLine();


			//TRANSLATE BUTTON
			if (operation == ImGuizmo::OPERATION::TRANSLATE)
				ImGui::PushStyleColor(ImGuiCol_Button, oldButtonColor);
			if (ImGui::Button(ICON_FK_ARROWS) && operation != ImGuizmo::OPERATION::TRANSLATE) operation = ImGuizmo::OPERATION::TRANSLATE;
			else if (operation == ImGuizmo::OPERATION::TRANSLATE)
				ImGui::PopStyleColor();

			ImGui::SameLine();

			//SCALE BUTTON
			if (operation == ImGuizmo::OPERATION::SCALE)
				ImGui::PushStyleColor(ImGuiCol_Button, oldButtonColor);
			if (ImGui::Button(ICON_FK_ARROWS_ALT) && operation != ImGuizmo::OPERATION::SCALE) operation = ImGuizmo::OPERATION::SCALE;
			else if (operation == ImGuizmo::OPERATION::SCALE)
				ImGui::PopStyleColor();

			ImGui::SameLine();

			//ROTATE BUTTON
			if (operation == ImGuizmo::OPERATION::ROTATE)
				ImGui::PushStyleColor(ImGuiCol_Button, oldButtonColor);
			if (ImGui::Button(ICON_FK_REPEAT) && operation != ImGuizmo::OPERATION::ROTATE) operation = ImGuizmo::OPERATION::ROTATE;
			else if (operation == ImGuizmo::OPERATION::ROTATE)
				ImGui::PopStyleColor();


			ImGui::PopStyleColor();

			ImGui::SameLine();

			//ImGui::Text("Average FPS: %f", 1/time);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();
			if (ShouldDrawGrid)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, {0,0,0,0});
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 4 });
			if (ImGui::Button(ICON_FK_TH))
			{
				ShouldDrawGrid = !ShouldDrawGrid;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Toggle Grid");

			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_CARET_DOWN))
			{
				ImGui::OpenPopup("##viiewport_grid_settings");
			}
			if (ImGui::BeginPopupContextItem("##viiewport_grid_settings"))
			{
				ImGui::Text("Grid Settings:");
				ImGui::Separator();
				ImGui::Text("Opacity    ");
				ImGui::SameLine();
				ImGui::SliderFloat("##viewport_grid_opactiy", &ComponentDebugGizmos::GridOpacity, 0.f, 1.f);

				ImGui::Text("Size          ");
				ImGui::SameLine();
				static bool link = true;
				if (link)
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				if (ImGui::Button(ICON_FK_LINK "##link_size"))
				{
					link = !link;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				int size[] = { ComponentDebugGizmos::GridSize.x, ComponentDebugGizmos::GridSize.y };
				if (ImGui::DragInt2("##viewport_grid_size", size, 1.f, 1, 0))
				{
					if (size[0] < 1)
						size[0] = 1;
					if (size[1] < 1)
						size[1] = 1;
					if (link)
					{
						if (ComponentDebugGizmos::GridSize.x != size[0])
						{
							ComponentDebugGizmos::GridSize.x = size[0];
							ComponentDebugGizmos::GridSize.y = size[0];
						}
						else
						{
							ComponentDebugGizmos::GridSize.x = size[1];
							ComponentDebugGizmos::GridSize.y = size[1];
						}
					}
					else
					{
						ComponentDebugGizmos::GridSize.x = size[0];
						ComponentDebugGizmos::GridSize.y = size[1];
					}
					
				}

				ImGui::Text("Snap        ");
				ImGui::SameLine();
				static bool linkSnap = true;
				if (linkSnap)
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				if (ImGui::Button(ICON_FK_LINK "##link_snap"))
				{
					linkSnap = !linkSnap;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				int snap[] = { ComponentDebugGizmos::SnapSize.x, ComponentDebugGizmos::SnapSize.y };
				if (ImGui::DragInt2("##viewport_grid_snap", snap, 1.f, 1, 0))
				{
					if (snap[0] < 1)
						snap[0] = 1;
					if (snap[1] < 1)
						snap[1] = 1;
					if (link)
					{
						if (ComponentDebugGizmos::SnapSize.x != snap[0])
						{
							ComponentDebugGizmos::SnapSize.x = snap[0];
							ComponentDebugGizmos::SnapSize.y = snap[0];
						}
						else
						{
							ComponentDebugGizmos::SnapSize.x = snap[1];
							ComponentDebugGizmos::SnapSize.y = snap[1];
						}
					}
					else
					{
						ComponentDebugGizmos::SnapSize.x = snap[0];
						ComponentDebugGizmos::SnapSize.y = snap[1];
					}

				}
				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Change Grid Settings");

			ImGui::PopStyleVar();

			ImGui::PopStyleColor();

			ImGui::SameLine();

			if (Viewport::SnapToGrid)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });

			if (ImGui::Button(ICON_FK_MAGNET "##snap_to_grid"))
				Viewport::SnapToGrid = !Viewport::SnapToGrid;

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Snap to Grid");

			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

			if (ComponentDebugGizmos::DrawAllGizmos)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 4 });
			if (ImGui::Button(ICON_FK_COGS))
			{
				ComponentDebugGizmos::DrawAllGizmos = !ComponentDebugGizmos::DrawAllGizmos;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Toggle All Gizmos");

			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_CARET_DOWN "##viewport_gizmos_settings_caret"))
			{
				ImGui::OpenPopup("##viiewport_gizmos_settings");
			}
			if (ImGui::BeginPopupContextItem("##viiewport_gizmos_settings"))
			{
				ImGui::Text("Gizmos Settings:");
				ImGui::Checkbox("Camera##gizmos_settings", &ComponentDebugGizmos::DrawCameraGizmos);
				ImGui::Checkbox("Box Collider##gizmos_settings", &ComponentDebugGizmos::DrawBoxCollider);
				
				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Change Gizmos Settings");

			ImGui::PopStyleVar();

			ImGui::PopStyleColor();
			ImGui::SameLine();


			if (ComponentDebugGizmos::DrawAllEntities)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });

			if (ComponentDebugGizmos::DrawAllEntities)
			{
				if (ImGui::Button(ICON_FK_USERS "##draw_all_entities"))
					ComponentDebugGizmos::DrawAllEntities = false;
			}
			else
			{
				if (ImGui::Button(ICON_FK_USER "##draw_all_entities"))
					ComponentDebugGizmos::DrawAllEntities = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Toggle Viewing All Entity Gizmos");

			ImGui::PopStyleColor();

			ImGui::BeginChild("##viewport_frame_buffer");

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			viewportPanelSize = { viewportPanelSize.x, viewportPanelSize.y - 5.f };

			if ((s_ViewportSize.x != viewportPanelSize.x || s_ViewportSize.y != viewportPanelSize.y) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
			{
				frame_buffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				s_ViewportSize = { viewportPanelSize.x , viewportPanelSize.y };
				EditorApp::Camera.SetSize(s_ViewportSize);
			}

			ImVec2 fb_pos = ImGui::GetCursorScreenPos();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });

			uint32_t textureID = frame_buffer->GetColorAttachmentRendererID();
			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ s_ViewportSize.x, s_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			auto pos = ImGui::GetCursorPos();
			viewportFocused = ImGui::IsWindowFocused();
			viewportHovered = ImGui::IsItemHovered();
			viewportMousePos = ImGui::GetMousePos();
			viewportMousePos = {Math::min(viewportMousePos.x - ImGui::GetWindowPos().x, ImGui::GetWindowSize().x), Math::min(viewportMousePos.y - ImGui::GetWindowPos().y, ImGui::GetWindowSize().y)};
			Math::vec2 flippedMousePos = { viewportMousePos.x, s_ViewportSize.y - viewportMousePos.y - 1 };
			static bool itemDrag = false;

			if (itemDrag && !ImGui::IsMouseDown(ImGuiMouseButton_Left) && Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
			{
				Math::vec2 nonSnapPos = EditorApp::CurrentScene->GetTransformAbsolutePosition(Hierarchy::SelectedEntity);
				EditorApp::CurrentScene->SetTransformAbsolutePosition(Hierarchy::SelectedEntity, { snapf(nonSnapPos.x, 1, 1), snapf(nonSnapPos.y, 1, 1) });
			}

			static Math::vec2 offset = { 0,0 };
			if ((!EditorApp::SceneRunning || EditorApp::ScenePaused) && ImGui::IsItemHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Left)))
			{
				frame_buffer->Bind();
				int pixelVal = frame_buffer->ReadPixel(1, flippedMousePos.x, flippedMousePos.y);
				frame_buffer->Unbind();
				//CHROMA_CORE_INFO("Selected Entity: {}", pixelVal);
				if (pixelVal > -1 && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					if (EditorApp::CurrentScene->Registry.valid((Chroma::EntityID)pixelVal))
					{
						itemDrag = true;
						Hierarchy::SelectedEntity = (Chroma::EntityID)pixelVal;
						offset = ViewportPositionToWorld({ flippedMousePos.x, flippedMousePos.y }) - EditorApp::CurrentScene->GetTransformAbsolutePosition(Hierarchy::SelectedEntity);
					}
				}
				else if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_CONTROL))
				{
					itemDrag = false;
					offset = { 0,0 };
					Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
				}
			}

			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))

			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				itemDrag = false;
				offset = { 0,0 };
			}
				

			

			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL && itemDrag && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && (ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_CONTROL) || operation == ImGuizmo::OPERATION::TRANSLATE))
			{
				Math::vec2 posToMove = ViewportPositionToWorld({ flippedMousePos.x, flippedMousePos.y }) - offset;
				bool doSnap = SnapToGrid && !ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_ALT);
				if (doSnap)
					EditorApp::CurrentScene->SetTransformAbsolutePosition(Hierarchy::SelectedEntity, { snapf(posToMove.x, ComponentDebugGizmos::SnapSize.x, 1), snapf(posToMove.y, ComponentDebugGizmos::SnapSize.y, 1)});
				else
					EditorApp::CurrentScene->SetTransformAbsolutePosition(Hierarchy::SelectedEntity, posToMove);
			}
				
			//ImGuizmo

			ComponentDebugGizmos::DrawIcons();
			EditorCamera::ImGuiUpdate();

			ImGui::SetCursorPos(pos);

			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
			{
				ImGuizmo::SetOrthographic(true);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(fb_pos.x, fb_pos.y, s_ViewportSize.x, s_ViewportSize.y);
				//auto& cam = dynamic_cast<EditorApp&>(EditorApp::Get()).GetCameraController();
				auto& cam = EditorApp::CurrentScene->GetPrimaryCamera();
				const Math::mat4& camView = cam.GetViewMatrix();
				const Math::mat4& camProj = cam.GetProjectionMatrix();

			

				auto &transform = EditorApp::CurrentScene->GetComponent<Chroma::Transform>(Hierarchy::SelectedEntity);
				glm::mat4 transformMat = transform.GetTransform();

				glm::vec2 snap;
				if (operation == ImGuizmo::OPERATION::SCALE)
					snap = { 0.01f, 0.01f };
				else
					snap = { 1.f, 1.f };
				//ImGuizmo::DrawGrid(glm::value_ptr(camView), glm::value_ptr(camProj), identityMatrix, 32.f);
				ImGuizmo::Manipulate(glm::value_ptr(camView), glm::value_ptr(camProj),
					operation, ImGuizmo::LOCAL, glm::value_ptr(transformMat), (float*)0, glm::value_ptr(snap));

				if (ImGuizmo::IsUsing())
				{

					glm::vec3 translation = { transform.Position, 0 };
					glm::vec3 rotation = { 0,0, transform.Rotation };
					glm::vec3 scale = { transform.Scale, 0 };

					if (Chroma::DecomposeTransform(transformMat, translation, rotation, scale))
					{
						//rotation.x = glm::degrees(rotation.x);
						//rotation.y = glm::degrees(rotation.y);
						//rotation.z = glm::degrees(rotation.z);


						glm::vec3 deltaRotation = rotation - transform.Rotation;

						//CHROMA_CORE_WARN("Position: [{},{},{}]", translation.x, translation.y, translation.z);
						//CHROMA_CORE_WARN("Rotation: [{},{},{}]", rotation.x, rotation.y, rotation.z);
						//CHROMA_CORE_WARN("Scale: [{},{},{}]", scale.x, scale.y, scale.z);

						transform.Position = { translation.x, translation.y };
						transform.Rotation += deltaRotation.z;
						transform.Scale = { scale.x, scale.y };
					}

					
				}
			}

			ImGui::PopStyleVar();
			ImGui::EndChild();

			ImGui::End();
		}
		
	}
	const Math::vec2 Viewport::ViewportPositionToWorld(const Math::vec2& pos)
	{
		glm::vec3 res = glm::unProject(glm::vec3( pos.x, pos.y, 0.f ), EditorApp::Camera.GetViewMatrix(), EditorApp::Camera.GetProjectionMatrix(), glm::vec4(0.f, 0.f, s_ViewportSize.x, s_ViewportSize.y ));
		return { res.x, res.y };
		
	}

	const Math::vec2 Viewport::WorldToViewportPosition(const Math::vec2& pos)
	{
		glm::vec3 res = glm::project(glm::vec3(pos.x, pos.y, 0.f), EditorApp::Camera.GetViewMatrix(), EditorApp::Camera.GetProjectionMatrix(), glm::vec4(0.f, 0.f, s_ViewportSize.x, s_ViewportSize.y));
		return { res.x, res.y };

	}
	bool Viewport::IsViewportFocused()
	{
		return viewportFocused;
	}

	bool Viewport::IsViewportHovered()
	{
		return viewportHovered;
	}
	ImVec2 Viewport::GetViewportMousePos()
	{
		return viewportMousePos;
	}
}