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
#include "ComponentDebugWidgets.h"

namespace Polychrome
{
	static glm::vec2 s_ViewportSize = glm::vec2(1270, 720);

	static bool shouldUpdate = true;
	static bool viewportFocused = false;
	static bool viewportHovered = false;
	static ImVec2 viewportMousePos = { 0,0 };
	static Chroma::Window* s_ViewportWindow;

	bool Viewport::Open = true;

	static const float identityMatrix[16] =
	{ 
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f 
	};


	void Viewport::Draw(Chroma::Ref<Chroma::Framebuffer> frame_buffer)
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

			uint32_t textureID = frame_buffer->GetColorAttachmentRendererID();


		
			//TOOLBAR

			static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;


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
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			if (ImGui::Button(ICON_FK_MOUSE_POINTER) && operation != ImGuizmo::OPERATION::BOUNDS)
				operation = ImGuizmo::OPERATION::BOUNDS;
			else if (operation == ImGuizmo::OPERATION::BOUNDS)
				ImGui::PopStyleColor();

			ImGui::SameLine();


			//TRANSLATE BUTTON
			if (operation == ImGuizmo::OPERATION::TRANSLATE)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			if (ImGui::Button(ICON_FK_ARROWS) && operation != ImGuizmo::OPERATION::TRANSLATE) operation = ImGuizmo::OPERATION::TRANSLATE;
			else if (operation == ImGuizmo::OPERATION::TRANSLATE)
				ImGui::PopStyleColor();

			ImGui::SameLine();

			//SCALE BUTTON
			if (operation == ImGuizmo::OPERATION::SCALE)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			if (ImGui::Button(ICON_FK_ARROWS_ALT) && operation != ImGuizmo::OPERATION::SCALE) operation = ImGuizmo::OPERATION::SCALE;
			else if (operation == ImGuizmo::OPERATION::SCALE)
				ImGui::PopStyleColor();

			ImGui::SameLine();

			//ROTATE BUTTON
			if (operation == ImGuizmo::OPERATION::ROTATE)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			if (ImGui::Button(ICON_FK_REPEAT) && operation != ImGuizmo::OPERATION::ROTATE) operation = ImGuizmo::OPERATION::ROTATE;
			else if (operation == ImGuizmo::OPERATION::ROTATE)
				ImGui::PopStyleColor();


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

			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ s_ViewportSize.x, s_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			auto pos = ImGui::GetCursorPos();
			viewportFocused = ImGui::IsWindowFocused();
			viewportHovered = ImGui::IsItemHovered();
			viewportMousePos = ImGui::GetMousePos();
			viewportMousePos = {Math::min(viewportMousePos.x - ImGui::GetWindowPos().x, ImGui::GetWindowSize().x), Math::min(viewportMousePos.y - ImGui::GetWindowPos().y, ImGui::GetWindowSize().y)};
			//ImGuizmo

			ComponentDebugWidgets::DrawIcons();
			EditorCamera::ImGuiUpdate();

			ImGui::SetCursorPos(pos);

			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
			{
				ImGuizmo::SetOrthographic(true);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(fb_pos.x, fb_pos.y, s_ViewportSize.x, s_ViewportSize.y);
				//auto& cam = dynamic_cast<EditorApp&>(EditorApp::Get()).GetCameraController();
				auto& cam = Chroma::CameraComponent::GetPrimaryCamera();
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