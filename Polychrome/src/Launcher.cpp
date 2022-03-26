#include "Launcher.h"
#include "Utilities/GLFWHelpers.h"
#include "thid_party/platform_folders.h"
#include <imgui.h>
#include "Fonts/IconsForkAwesome.h"
#include "Fonts/Roboto.cpp"
//#include "Style.h"
#include <Chroma/Utilities/FileDialogs.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include "thid_party/ProgressIndicators.h"
#include "EditorApp.h"

namespace Polychrome
{

	static ImFont* LargeIcons;
	static ImFont* LargeFont;
	static ImFont* HeaderFont;
	static ImFont* SmallIcons;

	std::atomic_bool Launcher::Loading = false;
	std::atomic_bool Launcher::Done = false;
	std::atomic_bool Launcher::Stopped = false;
	std::mutex Launcher::ProgressLock;
	std::string Launcher::ProgressMessage;
	float Launcher::Progress = 0.f;

	std::string Launcher::path;
	std::string Launcher::project_name;
	bool Launcher::create;
	std::string Launcher::starting_scene;



	std::vector<Polychrome::Launcher::RecentProjectInfo> recentProjects;


	Launcher::Launcher()
	{



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

	Launcher::~Launcher()
	{
		Stopped.store(true);
	}

	void Launcher::Init()
	{
		//ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 14.f);



		ImFontConfig config;
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
		ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 14.f, &config, icon_ranges);
		io.FontDefault = font;
		LargeIcons = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 128.0f, nullptr, icon_ranges);
		SmallIcons = io.Fonts->AddFontFromFileTTF("assets/fonts/forkawesome-webfont.ttf", 20.f, nullptr, icon_ranges);


		LargeFont = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 64.f);

		HeaderFont = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 20.f);


		ImGui::GetStyle().FrameRounding = 1;
	}


	void Launcher::ImGuiDraw(Chroma::Time time, float width, float height, void* nativeWindowHandle)
	{

		//ImGui::SetNextWindowPos({ 0,0 });
		//ImGui::SetNextWindowSize({ (float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight() });

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

		//ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos({0,0}, ImGuiCond_Appearing);

		ImGui::SetNextWindowSize({width, height});

		if (Loading)
		{
			ImGui::Begin("##LOADING", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize);

			while (!ProgressLock.try_lock())
			{
			}

			//ImGui::SameLine();

			//ImGui::Dummy({ 1, height / 2 });
			//ImGui::SetCursorPos({ height / 4 - 10, width / 2 - ImGui::CalcTextSize(ProgressMessage.c_str()).x / 2.f });

			ImGui::BeginGroup();
			ImGui::Dummy({ 1, 1 });
			ImGui::SameLine();
			ImGui::Text(ProgressMessage.c_str());

			const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
			const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

			ImGui::BufferingBar("##buffer_barrrr", Progress, ImVec2(ImGui::GetWindowWidth() - 60, 6), bg, col);

			ImGui::EndGroup();

			ProgressLock.unlock();

			ImGui::End();
		}
		else if (!Loading && !Done)
		{
			ImGui::Begin("##ProjectOpenWindow", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize);

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
			}
			ImGui::PopStyleColor();

			ImGui::Separator();

			if (!creating_new)
			{
				static std::vector<bool> hovered(recentProjects.size(), false);
				static std::vector<bool> active(recentProjects.size(), false);

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

					ImFont _font = *LargeIcons;
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

						//CLOSE AND START
						Loading.store(true);
						Done.store(false);

						Launcher::path = prj.Path;
						Launcher::project_name = prj.Name;
						Launcher::create = false;

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

					ImFont _font = *LargeIcons;
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

						//CLOSE AND START
						Loading.store(true);
						Done.store(false);

						Launcher::path = prj.Path;
						Launcher::project_name = prj.Name;
						Launcher::create = false;

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
					creating_path = sago::getDocumentsFolder() + "\\Chroma Projects\\";
					creating_starting = "New Scene";
				}
				ImGui::SameLine();
				if (ImGui::Button("Add an existing project..."))
				{
					std::string selectedFile = Chroma::FileDialogs::OpenFile("Polychrome Project (*.polychrome)\0*.polychrome\0", nativeWindowHandle);

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

				ImGui::InputText("##name_new_proj", &creating_name);


				ImGui::Dummy({ 1, 12 });
				ImGui::Text("Location");

				ImGui::InputText("##path_new_proj", &creating_path);



				ImGui::SameLine();
				ImGui::PopItemWidth();
				if (ImGui::Button("..."))
				{
					std::string selectedFile = Chroma::FileDialogs::OpenDirectory(nativeWindowHandle);
					if (!selectedFile.empty())
					{
						creating_path = selectedFile;
					}
				}
				ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
				ImGui::Dummy({ 1, 12 });
				ImGui::Text("Starting Scene Name:");

				ImGui::InputText("##starting_scene_new_proj", &creating_starting);


				ImGui::PopItemWidth();

				ImGui::Dummy({ 1, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing() * 1.8f });

				if (ImGui::Button("Cancel"))
				{
					creating_new = false;
				}
				ImGui::SameLine();

				//ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !std::filesystem::exists(std::filesystem::path(creating_path).parent_path()));
				if (ImGui::Button("Create"))
				{
					if (!std::filesystem::exists(std::filesystem::path(creating_path).parent_path()))
						std::filesystem::create_directories(std::filesystem::path(creating_path));
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
					e << YAML::Key << "Path" << YAML::Value << creating_path + creating_name;
					e << YAML::Key << "Pinned" << YAML::Value << false;
					e << YAML::EndMap;


					e << YAML::EndSeq << YAML::EndMap;

					//CLOSE AND START
					Loading.store(true);
					Done.store(false);

					Launcher::path = creating_path;
					Launcher::project_name = creating_name;
					Launcher::create = true;
					Launcher::starting_scene = creating_starting;
				}
				//ImGui::PopItemFlag();



			}



			ImGui::End();

		}


		ImGui::PopStyleVar();



		if (Done)
		{
			//glfwHideWindow((GLFWwindow*)this->GetWindow().GetNativeWindow());
			while (!ProgressLock.try_lock())
			{
			}
			ProgressMessage = "";
			Progress = 0.f;

			ProgressLock.unlock();
			Loading.store(false);
			//Stopped.store(true);
			//this->Stop();
		}
	}




}