#include "EditorApp.h"

#include "Viewport.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDrop.h"

#include <Chroma.h>
#include <Chroma/Core/EntryPoint.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include <fmod_studio.hpp>
#include <random>
#include "../../Chroma/third_party/GLFW/include/GLFW/glfw3.h"
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/CircleCollider.h>
#include <Chroma/Components/BoxCollider.h>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/Utilities/FileDialogs.h>
#include <filesystem>
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include "Fonts/Pinch.cpp"
#include "Fonts/Roboto.cpp"
#include <Chroma/Images/Aseprite.h>
#include "Style.h"
#include <time.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/Utilities/FileWatcher.h>
#include "readerwriterqueue.h"
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Systems/SpriteRendererSystem.h>
#include "AssetBrowser.h"
#include <Chroma/Scripting/MonoScripting.h>
#include "LogWindow.h"	
#include "Project.h"
#include "FileWatcherThread.h"
#include "ComponentDebugGizmos.h"
#include "ErrorWindow.h"
#include "Build.h"
#include "GameSettings.h"
#include "UndoRedo.h"

#define CHROMA_DEBUG_LOG
#include <Chroma/Core/Log.h>
#include "thid_party/platform_folders.h"
#include <chrono>
#include "Utilities/GLFWHelpers.h"
#include <Chroma/Components/Tag.h>
#include <mono/metadata/mono-gc.h>

namespace Polychrome
{

	

	Chroma::Scene* EditorApp::CurrentScene = nullptr;
	std::string EditorApp::CurrentScenePath;

	EditorCamera EditorApp::Camera;

	bool EditorApp::SceneRunning = false;
	bool EditorApp::ScenePaused = false;
	bool EditorApp::PreviewSprites = true;

	ImFont* EditorApp::LargeIcons = nullptr;
	ImFont* EditorApp::LargeFont = nullptr;

	GLFWwindow* temp;

	bool ProjectLoaded = false;

	std::string EditorApp::InfoMessage;
	EditorApp::MessageSeverity EditorApp::InfoSeverity = EditorApp::MessageSeverity::Info;

	struct RecentProjectInfo
	{
		std::string Name;
		std::string TimeStamp;
		std::string Path;
		bool Pinned = false;
	};

	std::vector<RecentProjectInfo> recentProjects;

	EditorApp::EditorApp()
		: Application("Polychrome Editor", 800U, 400U)
	{

#ifdef CHROMA_PLATFORM_WINDOWS
		glfwSetDropCallback((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), FileDrop::HandleFileDrop);
#endif
		//glfwHideWindow((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
		
		
		//glfwMakeContextCurrent(temp);
		//glfwDestroyWindow((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		//
		//glfwCreateWindow(800, 400, "", nullptr, temp);
		//glfwMakeContextCurrent((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
		glfwSetWindowCenter((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
		glfwSetWindowAttrib((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), GLFW_DECORATED, GLFW_FALSE);
		//this->GetWindow().SetVSync(true);
		

		std::filesystem::path appData(sago::getDataHome() + "/Polychrome");
		std::filesystem::path configFile(sago::getDataHome() + "/Polychrome/RecentProjects.yaml");

		if (!std::filesystem::exists(appData))
		{
			std::filesystem::create_directory(appData);
		}


		if (std::filesystem::exists(configFile))
		{
			auto ss = std::ostringstream{};
			std::ifstream stream(configFile);
			if (stream.good())
			{
				ss << stream.rdbuf();
				auto yaml = YAML::Load(ss.str());
				auto seq = yaml["RecentProjects"];
				if (seq)
				{
					for (auto project : seq)
					{
						RecentProjectInfo info;
						if (project["Name"])
						{
							info.Name = project["Name"].as<std::string>();
						}
						if (project["TimeStamp"])
						{
							info.TimeStamp = project["TimeStamp"].as<std::string>();
						}
						if (project["Path"])
						{
							info.Path = project["Path"].as<std::string>();
						}
						if (project["Pinned"])
						{
							info.Pinned = project["Pinned"].as<bool>();
						}
						if (std::filesystem::exists(info.Path + "\\" + info.Name + ".polychrome"))
						{
							recentProjects.push_back(info);
						}
							
					}
				}
			}
		}
			
	

	}

	void EditorApp::Init()
	{
		m_HighlightShader = Chroma::Shader::Create(".\\assets\\shaders\\Highlight.glsl");

		Chroma::FramebufferSpecification fbspec;
		fbspec.Width = 1920;
		fbspec.Height = 1080;
		fbspec.Attachments = { Chroma::FramebufferTextureFormat::RGBA8, Chroma::FramebufferTextureFormat::RED_INTEGER, Chroma::FramebufferTextureFormat::Depth };
		m_Framebuffer = Chroma::Framebuffer::Create(fbspec);
		m_GuizmoFramebuffer = Chroma::Framebuffer::Create(fbspec);

		// TODO: Load/Unload FMOD banks on play.
		//Chroma::Audio::LoadBank("assets/fmod/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
		//Chroma::Audio::LoadBank("assets/fmod/Desktop/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);


		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 14.f);
		
		

		ImFontConfig config;
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
		ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 14.f, &config, icon_ranges);
		io.FontDefault = font;
		LargeIcons = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 128.0f, nullptr, icon_ranges);

		LargeFont = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 64.f);


		ImGui::GetStyle().FrameRounding = 1;

		Config.Style = (int)ImGui::ImGuiStylePreset::ChromaDark;

		LogWindow::Init();

		Chroma::Input::SetGamepadConnectionCallback([](auto joystick) { CHROMA_CORE_WARN("Joystick Connected!"); });
		Chroma::Input::SetGamepadDisconnectionCallback([](auto joystick) { CHROMA_CORE_WARN("Joystick Disconnected!"); });


	}


	void EditorApp::Update(Chroma::Time time)
	{
		if (!ProjectLoaded)
			return;

		if (EditorApp::SceneRunning && !EditorApp::ScenePaused)
		{
			EditorApp::CurrentScene->Update(time);
		}
		else if (!EditorApp::SceneRunning && !EditorApp::ScenePaused)
		{
			sprite_system.m_Scene = EditorApp::CurrentScene;
			sprite_system.LateUpdate(time);
		}




		CHROMA_PROFILE_FUNCTION();
		// Update
		{
			CHROMA_PROFILE_SCOPE("CameraController OnUpdate");
			//m_CameraController.OnUpdate(time);
		}

		std::function<void()> func;
		bool success;
		while (success = FileWatcherThread::file_queue.try_dequeue(func))
		{
			if (success)
				func();
			else
				break;
		}
	}


	void EditorApp::Draw(Chroma::Time time)
	{

		CHROMA_PROFILE_SCOPE("Render OnUpdate");
		Chroma::RenderCommand::SetClearColor({ 0.0f, 0.0f , 0.0f , 1.0f });
		Chroma::Renderer2D::Clear();
		Chroma::Renderer2D::ResetStats();

		if (!ProjectLoaded)
			return;

		m_Framebuffer->Bind();

		static float rotation = 0.0f;
		rotation += time * 20.0f;

		Chroma::RenderCommand::SetClearColor({ 0.08f, 0.08f , 0.15f , 1.0f });
		Chroma::Renderer2D::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		Chroma::Renderer2D::Begin(Camera.GetViewProjectionMatrix());

		EditorApp::CurrentScene->Draw(time);

		Chroma::Renderer2D::End();

		m_Framebuffer->Unbind();

		m_GuizmoFramebuffer->Bind();

		Chroma::Renderer2D::Begin(Camera.GetViewProjectionMatrix());

		Chroma::RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 0.f });
		Chroma::Renderer2D::Clear();

		if (!EditorApp::SceneRunning || EditorApp::ScenePaused)
		{

			DrawSelectionMask(time);

			Chroma::Renderer2D::Flush(m_HighlightShader);
			Chroma::Renderer2D::StartBatch();
		}

		if (Viewport::ShouldDrawGrid)
			ComponentDebugGizmos::DrawGrid();
		ComponentDebugGizmos::DrawGizmos();

		Chroma::Renderer2D::End();

		m_GuizmoFramebuffer->Unbind();
	}

	static bool first = false;

	void EditorApp::ImGuiDraw(Chroma::Time time)
	{

		Camera.ImGuiUpdate();

		if (!first)
		{

			first = true;

			ImGui::OpenPopup("##ProjectOpenWindow");
		}


		if (!ProjectLoaded)
		{
			ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopup("##ProjectOpenWindow", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_Modal))
			{
				static bool creating_new = false;
				static std::string creating_name;
				static std::string creating_path;
				static std::string creating_starting;



				ImFont headerf = *LargeFont;
				headerf.Scale = headerf.Scale * .5f;
				ImGui::PushFont(&headerf);
				ImGui::Text("Polychrome Editor");
				ImGui::PopFont();

				ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - 24.f);
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				if (ImGui::Button(ICON_FK_TIMES, { 24, 24 }))
				{
					YAML::Emitter e;
					e << YAML::BeginMap;
					e << YAML::Key << "RecentProjects" << YAML::Value << YAML::BeginSeq;

					for (auto& proj : recentProjects)
					{
						e << YAML::BeginMap;
						e << YAML::Key << "Name" << YAML::Value << proj.Name;
						e << YAML::Key << "TimeStamp" << YAML::Value << proj.TimeStamp;
						e << YAML::Key << "Path" << YAML::Value << proj.Path;
						e << YAML::Key << "Pinned" << YAML::Value << proj.Pinned;
						e << YAML::EndMap;
					}

					e << YAML::EndSeq << YAML::EndMap;

					std::ofstream stream(sago::getDataHome() + "/Polychrome/RecentProjects.yaml");
					stream.write(e.c_str(), e.size());
					stream.close();
					this->Stop();
				}
				ImGui::PopStyleColor();

				ImGui::Separator();

				if (!creating_new)
				{
					static std::vector<bool> hovered(recentProjects.size(), false);
					static std::vector<bool> active(recentProjects.size(), false);

					//std::sort(recentProjects.begin(), recentProjects.end(), [](const RecentProjectInfo& a, const RecentProjectInfo& b) {
					//	std::istringstream str1(a.TimeStamp);
					//	std::istringstream str2(b.TimeStamp);
					//
					//	struct std::tm tm1;
					//	struct std::tm tm2;
					//
					//	str1 >> std::get_time(&tm1, "%d/%m/%Y %H:%M");
					//	str2 >> std::get_time(&tm2, "%d/%m/%Y %H:%M");
					//
					//	std::time_t time1 = mktime(&tm1);
					//	std::time_t time2 = mktime(&tm2);
					//
					//	return difftime(time1, time2) > 0.0;
					//
					//});


					// This caused memory errors in the Dist build for some reason :')
					//std::sort(recentProjects.begin(), recentProjects.end(), [](const RecentProjectInfo& a, const RecentProjectInfo& b) {
					//	return a.Pinned > b.Pinned;
					//});

					bool hasPinned = false;
					for (auto& prj : recentProjects)
					{
						if (prj.Pinned)
							hasPinned = true;
					}

					if (recentProjects.size() > 0 && hasPinned)
					{
						ImGui::Text("Pinned:");
					}

					int i = 0;
					for (auto& prj : recentProjects)
					{
						if (!prj.Pinned)
							continue;

						bool f = false;
						auto col = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
						if (active[i])
							col = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
						if (hovered[i] || active[i])
							ImGui::GetWindowDrawList()->AddRectFilled({ ImGui::GetCursorScreenPos().x - ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorScreenPos().y },
								ImVec2(ImGui::GetContentRegionAvailWidth() + ImGui::GetCursorScreenPos().x + ImGui::GetStyle().ItemSpacing.x - 30,
									ImGui::GetCursorScreenPos().y + ImGui::GetTextLineHeight() * 2 + ImGui::GetStyle().ItemSpacing.y * 2), col);

						ImGui::BeginGroup();

						float icon_size = ImGui::GetTextLineHeight() * 2 + ImGui::GetStyle().ItemSpacing.y * 2;

						ImFont _font = *EditorApp::LargeIcons;
						_font.Scale = icon_size / _font.FontSize;

						ImGui::PushFont(&_font);
						ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
						ImGui::Text(ICON_FK_CSHARP);
						ImGui::PopStyleColor(3);
						ImGui::PopFont();

						ImGui::SameLine();

						ImGui::BeginGroup();
						ImGui::Text(prj.Name.c_str());
						ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize(prj.TimeStamp.c_str()).x - 30);
						ImGui::Text(prj.TimeStamp.c_str());
						ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), prj.Path.c_str());
						ImGui::EndGroup();
						ImGui::EndGroup();

						if (ImGui::IsItemHovered())
							hovered[i] = true;
						else
							hovered[i] = false;

						if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
							active[i] = true;
						else
							active[i] = false;

						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
						{
							std::time_t t = std::time(nullptr);
							char buffer[80];
							strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&t));

							YAML::Emitter e;
							e << YAML::BeginMap;
							e << YAML::Key << "RecentProjects" << YAML::Value << YAML::BeginSeq;

							for (auto& proj : recentProjects)
							{
								e << YAML::BeginMap;
								e << YAML::Key << "Name" << YAML::Value << proj.Name;
								if (proj.Name == prj.Name && proj.Path == prj.Path)
									e << YAML::Key << "TimeStamp" << YAML::Value << std::string(buffer);
								else
									e << YAML::Key << "TimeStamp" << YAML::Value << proj.TimeStamp;
								
								e << YAML::Key << "Path" << YAML::Value << proj.Path;
								e << YAML::Key << "Pinned" << YAML::Value << proj.Pinned;
								e << YAML::EndMap;
							}

							e << YAML::EndSeq << YAML::EndMap;

							std::ofstream stream(sago::getDataHome() + "/Polychrome/RecentProjects.yaml");
							stream.write(e.c_str(), e.size());
							stream.close();

							Project::LoadProject(prj.Path + "\\" + prj.Name + ".polychrome");

							EditorApp::CurrentScenePath = Chroma::AssetManager::AssetDirectory + Project::StartingScene;

							glfwSetWindowSize((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), 1920, 1080);
							glfwSetWindowCenter((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
							glfwMaximizeWindow((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
							glfwSetWindowAttrib((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), GLFW_DECORATED, GLFW_TRUE);
							glfwSetWindowTitle((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), ("Polychrome Editor - " + prj.Name + " - " + std::filesystem::path(Project::StartingScene).filename().string()).c_str());

							Polychrome::FileWatcherThread::SetWatch(Chroma::AssetManager::AssetDirectory);

							ProjectLoaded = true;
							ImGui::ResetStyle(ImGui::ImGuiStylePreset::Cherry, ImGui::GetStyle());
							ImGui::CloseCurrentPopup();
						}

						ImGui::SameLine();
						ImGui::PushID(&prj.Path);
						ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
						if (ImGui::Button(ICON_FK_THUMB_TACK, { 30, 36 }))
						{
							prj.Pinned = !prj.Pinned;
						}
						ImGui::PopStyleColor();
						ImGui::PopID();

						i++;
					}

					if (hasPinned)
						ImGui::Separator();

					for (auto& prj : recentProjects)
					{
						if (prj.Pinned)
							continue;

						bool f = false;
						auto col = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
						if (active[i])
							col = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
						if (hovered[i] || active[i])
							ImGui::GetWindowDrawList()->AddRectFilled({ ImGui::GetCursorScreenPos().x - ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorScreenPos().y },
								ImVec2(ImGui::GetContentRegionAvailWidth() + ImGui::GetCursorScreenPos().x + ImGui::GetStyle().ItemSpacing.x - 30,
									ImGui::GetCursorScreenPos().y + ImGui::GetTextLineHeight() * 2 + ImGui::GetStyle().ItemSpacing.y * 2), col);

						ImGui::BeginGroup();

						float icon_size = ImGui::GetTextLineHeight() * 2 + ImGui::GetStyle().ItemSpacing.y * 2;

						ImFont _font = *EditorApp::LargeIcons;
						_font.Scale = icon_size / _font.FontSize;

						ImGui::PushFont(&_font);
						ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
						ImGui::Text(ICON_FK_CSHARP);
						ImGui::PopStyleColor(3);
						ImGui::PopFont();

						ImGui::SameLine();

						ImGui::BeginGroup();
						ImGui::Text(prj.Name.c_str());
						ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize(prj.TimeStamp.c_str()).x - 30);
						ImGui::Text(prj.TimeStamp.c_str());
						ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), prj.Path.c_str());
						ImGui::EndGroup();
						ImGui::EndGroup();

						if (ImGui::IsItemHovered())
							hovered[i] = true;
						else
							hovered[i] = false;

						if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
							active[i] = true;
						else
							active[i] = false;

						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
						{
							std::time_t t = std::time(nullptr);
							char buffer[80];
							strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&t));

							YAML::Emitter e;
							e << YAML::BeginMap;
							e << YAML::Key << "RecentProjects" << YAML::Value << YAML::BeginSeq;

							for (auto& proj : recentProjects)
							{
								e << YAML::BeginMap;
								e << YAML::Key << "Name" << YAML::Value << proj.Name;
								if (proj.Name == prj.Name && proj.Path == prj.Path)
									e << YAML::Key << "TimeStamp" << YAML::Value << std::string(buffer);
								else
									e << YAML::Key << "TimeStamp" << YAML::Value << proj.TimeStamp;

								e << YAML::Key << "Path" << YAML::Value << proj.Path;
								e << YAML::Key << "Pinned" << YAML::Value << proj.Pinned;
								e << YAML::EndMap;
							}

							e << YAML::EndSeq << YAML::EndMap;

							std::ofstream stream(sago::getDataHome() + "/Polychrome/RecentProjects.yaml");
							stream.write(e.c_str(), e.size());
							stream.close();

							Project::LoadProject(prj.Path + "\\" + prj.Name + ".polychrome");

							EditorApp::CurrentScenePath = Chroma::AssetManager::AssetDirectory + Project::StartingScene;

							glfwSetWindowSize((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), 1920, 1080);
							glfwSetWindowCenter((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
							glfwMaximizeWindow((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
							glfwSetWindowAttrib((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), GLFW_DECORATED, GLFW_TRUE);
							glfwSetWindowTitle((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), ("Polychrome Editor - " + prj.Name + " - " + std::filesystem::path(Project::StartingScene).filename().string()).c_str());

							Polychrome::FileWatcherThread::SetWatch(Chroma::AssetManager::AssetDirectory);

							ProjectLoaded = true;
							ImGui::ResetStyle(ImGui::ImGuiStylePreset::Cherry, ImGui::GetStyle());
							ImGui::CloseCurrentPopup();
						}

						ImGui::SameLine();
						ImGui::PushID(&prj.Path);
						ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
						if (ImGui::Button(ICON_FK_THUMB_TACK, { 30, 36 }))
						{
							prj.Pinned = !prj.Pinned;
						}
						ImGui::PopStyleColor();
						ImGui::PopID();

						i++;
					}

					ImGui::Dummy({ 1, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing() * 1.8f });

					ImGui::Separator();
					if (ImGui::Button("Create a new project..."))
					{
						creating_new = true;
						creating_name = "NewProject";
						creating_path = sago::getDocumentsFolder() + "\\ChromaProjects\\";
						creating_starting = "New Scene";
					}
					ImGui::SameLine();
					if (ImGui::Button("Add an existing project..."))
					{
						std::string selectedFile = Chroma::FileDialogs::OpenFile("Polychrome Project (*.polychrome)\0*.polychrome\0");

						if (!selectedFile.empty())
						{
							RecentProjectInfo info;
							auto patha = std::filesystem::path(selectedFile);

							info.Name = patha.filename().replace_extension("").string();
							info.Path = patha.parent_path().string();
							info.Pinned = false;
							std::time_t t = std::time(nullptr);
							char buffer[80];
							strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&t));
							info.TimeStamp = std::string(buffer);
							recentProjects.push_back(info);
							active.resize(active.size() + 1);
							active[active.size() - 1] = true;
							hovered.resize(hovered.size() + 1);
							hovered[hovered.size() - 1] = false;
						}
						
					}
				}
				else
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
					ImGui::Text("Name:");
					ImGui::InputText("##name_new_proj", &creating_name, ImGuiInputTextFlags_CharsNoBlank);
					ImGui::Dummy({ 1, 12 });
					ImGui::Text("Location");
					ImGui::InputText("##path_new_proj", &creating_path, ImGuiInputTextFlags_CharsNoBlank);
					ImGui::SameLine();
					ImGui::PopItemWidth();
					if (ImGui::Button("..."))
					{
						std::string selectedFile = Chroma::FileDialogs::OpenDirectory();
						if (!selectedFile.empty())
						{
							creating_path = selectedFile;
						}
					}
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
					ImGui::Dummy({ 1, 12 });
					ImGui::Text("Starting Scene Name:");
					ImGui::InputText("##starting_scene_new_proj", &creating_starting, ImGuiInputTextFlags_CharsNoBlank);

					ImGui::PopItemWidth();

					ImGui::Dummy({ 1, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing() * 1.8f });

					if (ImGui::Button("Cancel"))
					{
						creating_new = false;
					}
					ImGui::SameLine();

					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !std::filesystem::exists(std::filesystem::path(creating_path).parent_path()));
					if (ImGui::Button("Create"))
					{
						Project::CreateProject(creating_name, creating_path, "\\Scenes\\" + creating_starting);
						Chroma::AssetManager::AssetDirectory = creating_path + "\\" + creating_name + "\\Assets";
						this->CurrentScene = new Chroma::Scene();
						auto camera = this->CurrentScene->NewEntity();
						this->CurrentScene->SetPrimaryCamera(camera.GetID());
						this->CurrentScene->GetComponent<Chroma::Tag>(camera.GetID()).EntityName = "Camera";
						this->CurrentScene->Name = creating_starting;
						EditorApp::CurrentScenePath = Chroma::AssetManager::AssetDirectory + "\\Scenes\\" + creating_starting + ".chroma";
						SaveScene();

						std::time_t t = std::time(nullptr);
						char buffer[80];
						strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&t));

						YAML::Emitter e;
						e << YAML::BeginMap;
						e << YAML::Key << "RecentProjects" << YAML::Value << YAML::BeginSeq;

						for (auto& proj : recentProjects)
						{
							e << YAML::BeginMap;
							e << YAML::Key << "Name" << YAML::Value << proj.Name;
							e << YAML::Key << "TimeStamp" << YAML::Value << proj.TimeStamp;
							e << YAML::Key << "Path" << YAML::Value << proj.Path;
							e << YAML::Key << "Pinned" << YAML::Value << proj.Pinned;
							e << YAML::EndMap;
						}

						e << YAML::BeginMap;
						e << YAML::Key << "Name" << YAML::Value << creating_name;
						e << YAML::Key << "TimeStamp" << YAML::Value << std::string(buffer);
						e << YAML::Key << "Path" << YAML::Value << creating_path  + creating_name;
						e << YAML::Key << "Pinned" << YAML::Value << false;
						e << YAML::EndMap;


						e << YAML::EndSeq << YAML::EndMap;

						std::ofstream stream(sago::getDataHome() + "/Polychrome/RecentProjects.yaml");
						stream.write(e.c_str(), e.size());
						stream.close();

						glfwSetWindowSize((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), 1920, 1080);
						glfwSetWindowCenter((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
						glfwMaximizeWindow((GLFWwindow*)this->Get().GetWindow().GetNativeWindow());
						glfwSetWindowAttrib((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), GLFW_DECORATED, GLFW_TRUE);
						glfwSetWindowTitle((GLFWwindow*)this->Get().GetWindow().GetNativeWindow(), ("Polychrome Editor - " + creating_name + " - " + creating_starting).c_str());

						Polychrome::FileWatcherThread::SetWatch(Chroma::AssetManager::AssetDirectory);

						ProjectLoaded = true;
						ImGui::ResetStyle(ImGui::ImGuiStylePreset::Cherry, ImGui::GetStyle());
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopItemFlag();
					


				}

				
					
				ImGui::End();
			}
			
			ImGui::PopStyleVar();

			return;
		}
			

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("MAIN_WINDOW", (bool*)0, window_flags);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.FramePadding.x = 4;
		style.FramePadding.y = 3;

		style.WindowMinSize.x = 100.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImVec2 avail = ImGui::GetContentRegionAvail();
			ImGui::DockSpace(dockspace_id, ImVec2(avail.x, avail.y - ImGui::GetTextLineHeight() - style.FramePadding.y * 2), ImGuiDockNodeFlags_None);
		}

		style.WindowMinSize.x = minWinSizeX;

		int prevFramePaddingY = (int)ImGui::GetStyle().FramePadding.y;
		ImGui::GetStyle().FramePadding.y = 4.0f;

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File##MAIN_MENU_BAR"))
		{
			ImGui::MenuItem("New##MAIN_MENU_BAR");
			if (ImGui::MenuItem("Open...##MAIN_MENU_BAR", "Ctrl+O"))
				OpenScene();

			if (ImGui::MenuItem("Save##MAIN_MENU_BAR", "Ctrl+S"))
			{
				SaveScene();
				Project::SaveCurrentProject();
			}
				

			if (ImGui::MenuItem("Save As...##MAIN_MENU_BAR", "Ctrl+Shift+S"))
			{
				SaveSceneAs();
				Project::SaveCurrentProject();
			}
				
			
			ImGui::MenuItem("Close##MAIN_MENU_BAR", "Alt + F4");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("Undo##MAIN_MENU_BAR", "Ctrl+Z", false, UndoRedo::CanUndo()))
			{
				//UNDO
				UndoRedo::Undo();
			}
			if (ImGui::MenuItem("Redo##MAIN_MENU_BAR", "Ctrl+Y", false, UndoRedo::CanRedo()))
			{
				//REDO
				UndoRedo::Redo();
			}

			ImGui::EndMenu();
		}


		if ((ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_CONTROL) || ImGui::IsKeyDown((int)Chroma::Input::Key::RIGHT_CONTROL)) && ImGui::IsKeyPressed((int)Chroma::Input::Key::Z, false) && UndoRedo::CanUndo())
			UndoRedo::Undo();

		if ((ImGui::IsKeyDown((int)Chroma::Input::Key::LEFT_CONTROL) || ImGui::IsKeyDown((int)Chroma::Input::Key::RIGHT_CONTROL)) && ImGui::IsKeyPressed((int)Chroma::Input::Key::Y, false) && UndoRedo::CanRedo())
			UndoRedo::Redo();


		if (ImGui::BeginMenu("Object##MAIN_MENU_BAR"))
		{
			if (ImGui::MenuItem("New Entity##MAIN_MENU_BAR"))
			{
				CurrentScene->NewEntity();
			}
			if (ImGui::BeginMenu("New Entity...##MAIN_MENU_BAR"))
			{
				for (auto& component : Chroma::ECS::GetComponentNames())
				{
					if (component == "Transform" || component == "Relationship" || component == "Tag")
						continue;

					if (ImGui::MenuItem(component.c_str()))
					{
						Chroma::Entity ent = CurrentScene->NewEntity();
						ent.AddComponent(component);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Add a script", Hierarchy::SelectedEntity != Chroma::ENTITY_NULL))
			{
				for (auto& mod : Chroma::MonoScripting::GetModules())
				{
					if (ImGui::MenuItem(mod.c_str()))
					{
						if (CurrentScene->HasComponent<Chroma::CSharpScript>(Hierarchy::SelectedEntity))
						{
							auto child = CurrentScene->NewChild(Hierarchy::SelectedEntity);
							child.GetComponent<Chroma::Tag>().EntityName = mod + " Entity";
							Chroma::CSharpScript& script = child.AddComponent<Chroma::CSharpScript>();
							script.ModuleName = mod;
							Chroma::MonoScripting::InitScriptEntity(child);
						}
						else
						{
							Chroma::CSharpScript& script = CurrentScene->AddComponent<Chroma::CSharpScript>(Hierarchy::SelectedEntity);
							script.ModuleName = mod;
							Chroma::MonoScripting::InitScriptEntity(Chroma::Entity(Hierarchy::SelectedEntity, CurrentScene));
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		static bool PreferencesWindowOpen = false;

		if (ImGui::BeginMenu("Window##MAIN_MENU_BAR"))
		{
			ImGui::MenuItem("Hierarchy##MAIN_MENU_BAR", "", &Hierarchy::Open);
			ImGui::MenuItem("Inspector##MAIN_MENU_BAR", "", &Inspector::Open);
			ImGui::MenuItem("Viewport##MAIN_MENU_BAR", "", &Viewport::Open);
			ImGui::MenuItem("Log##MAIN_MENU_BAR", "", &LogWindow::Open);
			ImGui::MenuItem("Errors##MAIN_MENU_BAR", "", &ErrorWindow::Open);
			ImGui::MenuItem("Game Settings##MAIN_MENU_BAR", "", &GameSettings::Open);
			ImGui::MenuItem("Preferences##MAIN_MENU_BAR", "", &PreferencesWindowOpen);


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		ImGui::GetStyle().FramePadding.y = prevFramePaddingY;

		//ImGui::PopStyleVar();

		ImGui::ShowDemoWindow();

		Hierarchy::Draw();
		Inspector::Draw();
		Viewport::Draw(time, m_Framebuffer, m_GuizmoFramebuffer);
		LogWindow::Draw();
		ErrorWindow::Draw();
		AssetBrowser::Draw();
		GameSettings::Draw();


		if (ImGui::Begin("Script Engine Debug"))
		{
			float heap = (float)mono_gc_get_heap_size();
			float usage = (float)mono_gc_get_used_size();

			ImGui::Text("GC Heap: %.2fkb / %.2fkb", usage, heap);
			ImGui::Separator();

			for (auto& [scene, entityMap] : Chroma::MonoScripting::GetEntityInstanceMap())
			{
				ImGui::PushID(scene.ToString().c_str());
				if (ImGui::TreeNode(("Scene " + scene.ToString()).c_str()))
				{
					for (auto& [entityID, entityInstanceData] : entityMap)
					{
						Chroma::Entity entity = Chroma::Entity(entityID, CurrentScene);
						std::string entityName = "Unnamed Entity";
						if (entity.HasComponent<Chroma::Tag>())
							entityName = entity.GetComponent<Chroma::Tag>().EntityName;

						ImGui::PushID(entityName.c_str());
						if (ImGui::TreeNode(entityName.c_str()))
						{
							auto& script = entity.GetComponent<Chroma::CSharpScript>();
							for (auto& [moduleName, fieldMap] : script.ModuleFieldMap)
							{
								ImGui::PushID(moduleName.c_str());
								if (ImGui::TreeNode(moduleName.c_str()))
								{
									for (auto& [fieldName, field] : fieldMap)
									{
										ImGui::PushID(fieldName.c_str());
										if (ImGui::TreeNodeEx(fieldName.c_str(), ImGuiTreeNodeFlags_Leaf))
										{
											ImGui::TreePop();
										}
										ImGui::PopID();
									}
									ImGui::TreePop();
								}
								ImGui::PopID();
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
		ImGui::End();



		ImGui::Begin("Controller");

		//Chroma::Input::GamepadState state = Chroma::Input::GetGamepadState();
		//ImGui::Text(fmt::format("A:				{}", (int)state[Chroma::Input::GamepadButton::A]).c_str());
		//ImGui::Text(fmt::format("B:				{}", (int)state[Chroma::Input::GamepadButton::B]).c_str());
		//ImGui::Text(fmt::format("X:				{}", (int)state[Chroma::Input::GamepadButton::X]).c_str());
		//ImGui::Text(fmt::format("Y:				{}", (int)state[Chroma::Input::GamepadButton::Y]).c_str());
		//ImGui::Text(fmt::format("Left Bumper:	{}", (int)state[Chroma::Input::GamepadButton::LEFT_BUMPER]).c_str());
		//ImGui::Text(fmt::format("Right Bumper:	{}", (int)state[Chroma::Input::GamepadButton::RIGHT_BUMPER]).c_str());
		//ImGui::Text(fmt::format("Back:			{}", (int)state[Chroma::Input::GamepadButton::BACK]).c_str());
		//ImGui::Text(fmt::format("Start:			{}", (int)state[Chroma::Input::GamepadButton::START]).c_str());
		//ImGui::Text(fmt::format("Guide:			{}", (int)state[Chroma::Input::GamepadButton::GUIDE]).c_str());
		//ImGui::Text(fmt::format("Left Thumb:	{}", (int)state[Chroma::Input::GamepadButton::LEFT_THUMB]).c_str());
		//ImGui::Text(fmt::format("Right Thumb:	{}", (int)state[Chroma::Input::GamepadButton::RIGHT_THUMB]).c_str());
		//ImGui::Text(fmt::format("DPAD_UP:		{}", (int)state[Chroma::Input::GamepadButton::DPAD_UP]).c_str());
		//ImGui::Text(fmt::format("DPAD_DOWN:		{}", (int)state[Chroma::Input::GamepadButton::DPAD_DOWN]).c_str());
		//ImGui::Text(fmt::format("DPAD_LEFT:		{}", (int)state[Chroma::Input::GamepadButton::DPAD_LEFT]).c_str());
		//ImGui::Text(fmt::format("DPAD_RIGHT:	{}", (int)state[Chroma::Input::GamepadButton::DPAD_RIGHT]).c_str());
		ImGui::Text(fmt::format("A:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::A)).c_str());
		ImGui::Text(fmt::format("B:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::B)).c_str());
		ImGui::Text(fmt::format("X:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::X)).c_str());
		ImGui::Text(fmt::format("Y:				{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::Y)).c_str());
		ImGui::Text(fmt::format("Left Bumper:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::LEFT_BUMPER)).c_str());
		ImGui::Text(fmt::format("Right Bumper:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::RIGHT_BUMPER)).c_str());
		ImGui::Text(fmt::format("Back:			{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::BACK)).c_str());
		ImGui::Text(fmt::format("Start:			{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::START)).c_str());
		ImGui::Text(fmt::format("Guide:			{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::GUIDE)).c_str());
		ImGui::Text(fmt::format("Left Thumb:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::LEFT_THUMB)).c_str());
		ImGui::Text(fmt::format("Right Thumb:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::RIGHT_THUMB)).c_str());
		ImGui::Text(fmt::format("DPAD_UP:		{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_UP)).c_str());
		ImGui::Text(fmt::format("DPAD_DOWN:		{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_DOWN)).c_str());
		ImGui::Text(fmt::format("DPAD_LEFT:		{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_LEFT)).c_str());
		ImGui::Text(fmt::format("DPAD_RIGHT:	{}", (bool)Chroma::Input::GetGamepadButtonState(Chroma::Input::GamepadButton::DPAD_RIGHT)).c_str());

		ImGui::End();
		//Chroma::ImGuiDebugMenu::Draw();

		if (PreferencesWindowOpen)
		{
			if (ImGui::Begin("Preferences", &PreferencesWindowOpen))
			{
				static int selected_style = Config.Style;
				ImGui::SelectStyleCombo("Style", &selected_style, ImGui::ImGuiStyle_Count, nullptr);

			}
			ImGui::End();
		}

		if (InfoSeverity == MessageSeverity::Error)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.15f, 1.f));
			ImGui::Text(ICON_FK_EXCLAMATION_CIRCLE);
		}
		else if (InfoSeverity == MessageSeverity::Warning)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.1f, 1.f));
			ImGui::Text(ICON_FK_EXCLAMATION_TRIANGLE);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.3f, 0.8f, 1.f));
			ImGui::Text(ICON_FK_INFO_CIRCLE);
		}
		ImGui::SameLine();
		ImGui::Text(InfoMessage.c_str());
		ImGui::PopStyleColor();







		



		ImGui::End();


	}


	void EditorApp::NewScene()
	{

	}

	void EditorApp::OpenScene()
	{
		std::string filepath = Chroma::FileDialogs::OpenFile("Chroma Scene (*.chroma)\0*.chroma\0");
		if (!filepath.empty())
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;
			strStream << stream.rdbuf();
			Chroma::Scene* out = new Chroma::Scene();
			if (Chroma::Scene::Deserialize(out, strStream.str()))
			{
				CurrentScenePath = filepath;
				delete EditorApp::CurrentScene;
				EditorApp::CurrentScene = out;
				Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
			}
			else
			{
				delete out;
			}
		}
	}
	void EditorApp::SaveSceneAs()
	{
		std::string filepath = Chroma::FileDialogs::SaveFile("Chroma Scene (*.chroma)\0*.chroma\0");
		if (!filepath.empty())
		{
			if (!std::filesystem::path(filepath).has_extension())
				filepath += ".chroma";
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(filepath);
			fout2 << yaml;
			CurrentScenePath = filepath;
		}
	}

	void EditorApp::SaveScene()
	{
		//CHROMA_CORE_TRACE("{}", CurrentScenePath);
		if (!CurrentScenePath.empty() && std::filesystem::exists(CurrentScenePath))
		{
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(CurrentScenePath);
			fout2 << yaml;
		}
		else if (!CurrentScenePath.empty())
		{
			std::filesystem::create_directory(std::filesystem::path(CurrentScenePath).parent_path());
			std::string yaml = EditorApp::CurrentScene->Serialize();
			std::ofstream fout2(CurrentScenePath);
			fout2 << yaml;
		}
	}

	void EditorApp::OnEvent(Chroma::Event& e)
	{
		Chroma::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Chroma::MouseScrolledEvent>(CHROMA_BIND_EVENT_FN(EditorApp::OnMouseScrolled));
	}


	Math::vec2 lerp(Math::vec2 x, Math::vec2 y, float t)
	{
		return x * (1.f - t) + y * t;
	}

	bool EditorApp::OnMouseScrolled(Chroma::MouseScrolledEvent& e)
	{
		if (Viewport::IsViewportFocused() && Viewport::IsViewportHovered())
		{
			float zoom = Camera.GetZoom();
			zoom += e.getYOffset() * zoom / 8.0f;
			Camera.SetZoom(zoom);
			return true;
		}
		return false;
	}


	void EditorApp::DrawSelectionMask(Chroma::Time time)
	{
		static float totalTime = time;
		totalTime += time.GetSeconds();
		float offset = (Math::sin(4.f * totalTime) + 1.f);

		//CHROMA_CORE_INFO("offset: {}", offset);

		if (Hierarchy::SelectedEntity == Chroma::ENTITY_NULL || !EditorApp::CurrentScene->HasComponent<Chroma::SpriteRenderer>(Hierarchy::SelectedEntity))
			return;

		Chroma::Transform& transform = EditorApp::CurrentScene->Registry.get<Chroma::Transform>(Hierarchy::SelectedEntity);
		Chroma::SpriteRenderer& spriteRenderer = EditorApp::CurrentScene->Registry.get<Chroma::SpriteRenderer>(Hierarchy::SelectedEntity);
		Chroma::Relationship& relationship = EditorApp::CurrentScene->Registry.get<Chroma::Relationship>(Hierarchy::SelectedEntity);
		
		const Math::vec2& origin = spriteRenderer.GetSpriteOriginVector();
		
		if (Chroma::AssetManager::HasSprite(spriteRenderer.GetSpriteID()))
		{
			Chroma::Ref<Chroma::Sprite> s = Chroma::AssetManager::GetSprite(spriteRenderer.GetSpriteID());
			int w = s->Frames[spriteRenderer.GetCurrentFrame()].Texture->GetWidth();
			int h = s->Frames[spriteRenderer.GetCurrentFrame()].Texture->GetHeight();
			if (!relationship.IsChild())
			{
				Math::vec2 size = transform.Scale * Math::vec2((float)w, (float)h);
				Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };


				Chroma::Renderer2D::DrawQuad(transform.Position + spriteRenderer.Offset + originAdjustment, transform.Scale * Math::vec2((float)w + offset, (float)h + offset), s->Frames[spriteRenderer.GetCurrentFrame()].Texture, {0.f,1.f,1.f,1.f}, transform.Rotation);
			}
			else
			{
				Math::vec2 pos = transform.Position;
				Math::vec2 scale = transform.Scale;
				Math::vec2 parentPos{ 0,0 };
				float parentRot = 0;
				float rotation = transform.Rotation;
				Chroma::EntityID parent = relationship.Parent;
				while (parent != Chroma::ENTITY_NULL)
				{
					Chroma::Transform& parentTransform = EditorApp::CurrentScene->GetComponent<Chroma::Transform>(parent);
					parentPos += parentTransform.Position;
					scale *= parentTransform.Scale;
					parentRot += parentTransform.Rotation;
					parent = EditorApp::CurrentScene->GetComponent<Chroma::Relationship>(parent).Parent;
				}

				Math::vec2 adjusted = { pos.x * Math::cos(parentRot) - pos.y * Math::sin(parentRot), pos.x * Math::sin(parentRot) + pos.y * Math::cos(parentRot) };
				//CHROMA_CORE_TRACE("Adjusted: [{}, {}]; ParentPos: [{}, {}]; ParentRot: {}", adjusted.x, adjusted.y, parentPos.x, parentPos.y, parentRot);
				Math::vec2 size = scale * Math::vec2((float)w, (float)h);
				Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };

				
				Chroma::Renderer2D::DrawQuad(parentPos + adjusted + spriteRenderer.Offset + originAdjustment, scale * Math::vec2((float)w + offset, (float)h + offset), s->Frames[spriteRenderer.GetCurrentFrame()].Texture, { 0.f,1.f,1.f,1.f }, rotation + parentRot);
			}

		}
	}



	EditorApp::~EditorApp()
	{
		delete EditorApp::CurrentScene;
		if (FileWatcherThread::file_watcher_thread_running)
		{
			FileWatcherThread::file_watcher_thread_running.store(false);
			FileWatcherThread::file_watcher_thread.join();
		}

	}
}

Chroma::Application* Chroma::CreateApplication()
{
	return new Polychrome::EditorApp();
}

