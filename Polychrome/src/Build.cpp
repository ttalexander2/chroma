#include "Build.h"
#include <filesystem>
#include <Chroma/Scripting/MonoScripting.h>
#include <regex>
#include "ErrorWindow.h"
#include "EditorApp.h"
#include "Project.h"
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include "thid_party/ProgressIndicators.h"
#include <readerwriterqueue.h>
#include <Chroma/Utilities/FileDialogs.h>
#include <miniz.h>

namespace Polychrome
{
	std::vector<Build::BuildMessage> Build::BuildMessages = std::vector<Build::BuildMessage>();
	size_t Build::Errors = 0;
	size_t Build::Warnings = 0;
	size_t Build::Messages = 0;

	static bool _success = false;

	//Building Game
	static std::atomic_bool building = false;
	static std::thread buildThread;
	static std::mutex progressLock;
	static std::string progressMessage; //Used by multiple threads (use mutex first)
	static float progress = 0;  //Used by multiple threads (use mutex first)
	static std::atomic_bool done = false;

	//TODO: Build on a separate thread. Reload on dll change.
	bool Build::BuildMonoAssembly(const std::string& path, const std::string& name, bool debug, bool load_assembly)
	{
		_success = false;

		CHROMA_CORE_INFO("Building: {}", path + "\\bin\\" + name + ".dll");
		if (!std::filesystem::exists(path + "\\bin\\"))
			std::filesystem::create_directories(path + "\\bin\\");

		//If there's a better (still portable) way to do this, i would prefer that lol
		//Also this bat file assumes mono is installed (it uses system variables), so it should probably change anyways
		std::string monoLibStr = "";
		for (auto& p : std::filesystem::recursive_directory_iterator("mono\\lib\\mono\\4.5\\"))
		{
			if (p.path().extension() == ".dll")
			{
				monoLibStr += " -r:\"" + p.path().string() + "\"";
			}
		}

		std::string debugstr = "";
		if (debug)
			debugstr = "-debug ";


		std::string logFile = path + "\\bin\\" + "build.log";
		int success = system((".\\mono\\bin\\mono.exe mono\\lib\\mono\\4.5\\mcs.exe " + debugstr + "-target:library -nostdlib -out:\"" + std::filesystem::absolute(path).string() + "\\bin\\" + name + ".dll\" -r:Chroma.Mono.dll" + monoLibStr + " -recurse:\"" + std::filesystem::absolute(path).string() + "\\**.cs\" 2> \"" + logFile + "\" 1>&2").c_str());

		//CHROMA_CORE_TRACE("{}", (".\\mono\\bin\\mono.exe mono\\lib\\mono\\4.5\\mcs.exe -debug -target:library -nostdlib -out:\"" + std::filesystem::absolute(path).string() + "\\bin\\" + name + ".dll\" -r:Chroma.Mono.dll" + monoLibStr + " -recurse:\"" + std::filesystem::absolute(path).string() + "\\**.cs\" 2> \"" + logFile + "\" 1>&2").c_str());
		//CHROMA_CORE_TRACE("RESULT: {}", success);

		if (load_assembly)
		{
			std::ifstream stream = std::ifstream(logFile);
			//CHROMA_CORE_TRACE("log stream good?: {}", stream.good());
			stream.good();
			ParseErrors(stream);
			stream.close();
		}

		if (load_assembly)
			_success = (success == 0);


		if (load_assembly)
		{
			//CHROMA_CORE_INFO("Loading DLL: [{}]", path + "\\bin\\" + name + ".dll");
			if (std::filesystem::exists(path + "\\bin\\" + name + ".dll"))
				return Chroma::MonoScripting::LoadAppAssembly(path + "\\bin\\" + name + ".dll") && success == 0;
			else
				CHROMA_CORE_ERROR("DLL Not Found! [{}]", path + "\\bin\\" + name + ".dll");
		}


		return success == 0;
	}

	void Build::DrawBuildWindow()
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(600, 300));
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.14f, 0.17f, 1.00f));
		if (ImGui::BeginPopup("Build##BUILD_WINDOW", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_Modal))
		{

			static bool debug = false;
			static bool copy_mdb = false;
			static bool pack_assets = true;
			static std::string project_name = Project::Name;
			static std::string location_path = Project::Path + "\\bin\\";

			if (building)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::PushFont(EditorApp::HeaderFont);
			ImGui::Text("Build");
			ImGui::PopFont();

			ImGui::Text("Location: ");
			ImGui::SameLine();

			std::string input = std::filesystem::absolute(std::filesystem::path(location_path)).string().c_str();
			ImGui::InputText("##input_string_for_build_location", &input, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				std::string dir = Chroma::FileDialogs::OpenDirectory();
				if (!dir.empty())
					location_path = dir;
			}

			ImGui::Checkbox("Debug", &debug);
			ImGui::Checkbox("Copy MDB/PDB Files", &copy_mdb);
			ImGui::Checkbox("Pack Assets", &pack_assets);

			ImGui::Separator();

			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 30.f);

			bool button_pressed = false;
			if (ImGui::Button("Build"))
			{
				button_pressed = true;
				building.store(true);
				buildThread = std::thread(ExecuteGameBuild, debug, copy_mdb, pack_assets, project_name, location_path);
			}

			ImGui::SameLine();


			if (ImGui::Button("Close"))
			{
				progressMessage.clear();
				ImGui::CloseCurrentPopup();
			}

			if (building && !button_pressed)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			if (building)
			{
				while (!progressLock.try_lock())
				{
				}

				ImGui::SameLine();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10);

				ImGui::BeginGroup();
				ImGui::Dummy({ 10,1 });
				ImGui::SameLine();
				ImGui::Text(progressMessage.c_str());

				const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
				const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

				ImGui::BufferingBar("##buffer_bar", progress, ImVec2(ImGui::GetWindowWidth() - 60, 6), bg, col);

				ImGui::EndGroup();

				progressLock.unlock();

			}
			else
			{
				ImGui::SameLine();
				ImGui::Text(progressMessage.c_str());
			}

			if (done)
			{
				debug = false;
				copy_mdb = false;
				pack_assets = true;
				std::string project_name = Project::Name;
				building.store(false);
				done.store(false);
				progress = 0;
				buildThread.detach();
			}

			ImGui::EndPopup();

		}
		//ImGui::PopStyleColor();
}

	void Build::ExecuteGameBuild(bool debug, bool copy_mdb, bool pack_assets, const std::string& project_name, const std::string& location)
	{
		while (!progressLock.try_lock()){}

		progressMessage = "Copying runtime to target directory...";
		progress = 0.0f;
		progressLock.unlock();
		
		std::this_thread::sleep_for(std::chrono::seconds(1));

		while (!progressLock.try_lock()){}

		progressMessage = "Building C# Script Modules...";
		progress = 0.2f;
		progressLock.unlock();

		std::filesystem::create_directories(location + "\\" + project_name);
		Build::BuildMonoAssembly(Project::Path, project_name, debug, false);
		std::filesystem::copy_file(Project::Path + "\\bin\\" + project_name + ".dll", location + "\\" + project_name + "\\" + project_name + ".dll", std::filesystem::copy_options::overwrite_existing);

		if (debug && copy_mdb)
		{
			std::filesystem::copy_file(Project::Path + "\\bin\\" + project_name + ".dll.mdb", location + "\\" + project_name + "\\" + project_name + ".dll.mdb", std::filesystem::copy_options::overwrite_existing);
		}

		if (pack_assets)
		{
			while (!progressLock.try_lock()) {}
			progressMessage = "Packing assets...";
			progress = .35f;
			progressLock.unlock();

			CHROMA_CORE_ASSERT(false, "Packaging is currently unsupported!");

		}
		
		while (!progressLock.try_lock()){}

		progressMessage = "Copying assets...";
		progress = 0.5f;
		progressLock.unlock();

		std::filesystem::create_directories(location + "\\" + project_name + "\\Assets\\");

		std::filesystem::path newDir = std::filesystem::path(location + "\\" + project_name + "\\Assets\\");
		std::filesystem::path assetDir = std::filesystem::path(Project::Path + "\\Assets\\");
		for (auto& file : std::filesystem::recursive_directory_iterator(assetDir))
		{
			if (file.is_regular_file() && file.path().extension() == ".cs")
				continue;
			auto relativePath = std::filesystem::relative(file.path(), assetDir);
			if (file.is_directory())
			{
				std::filesystem::create_directories(newDir.string() + "\\" + relativePath.string());
			}
			else if (file.is_regular_file())
			{
				std::filesystem::create_directories(newDir.string() + "\\" + relativePath.parent_path().string());
				std::filesystem::copy_file(file.path(), newDir.string() + "\\" + relativePath.string(), std::filesystem::copy_options::overwrite_existing);
			}
				
		}

		//std::filesystem::copy(Project::Path + "\\Assets\\", location + "\\" + project_name + "\\Assets\\", std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

		while (!progressLock.try_lock())
		{
		}

		progressMessage = "Constructing additional pylons...";
		progress = 0.8f;
		progressLock.unlock();

		std::filesystem::copy_file(Project::Path + "\\" + Project::Name + ".polychrome", location + "\\" + project_name + "\\" + Project::Name + ".polychrome", std::filesystem::copy_options::overwrite_existing);

		std::this_thread::sleep_for(std::chrono::seconds(2));

		building.store(false);
		done.store(true);

		while (!progressLock.try_lock())
		{
		}

		progressMessage = "Done!";
		progress = 1.f;
		progressLock.unlock();
	}

	Build::BuildMessage* GetFirstOfType(Build::BuildMessage::Severity severity)
	{
		for (auto& message : Build::BuildMessages)
		{
			if (message.severity == severity)
				return &message;
		}
	}

	void Build::ParseErrors(std::ifstream& stream)
	{
		BuildMessages.clear();
		Errors = 0;
		Warnings = 0;
		Messages = 0;
		std::string line;
		while (std::getline(stream, line))
		{
			if (line.starts_with("Compilation succeeded"))
			{

				CHROMA_CORE_INFO("{}", line);
				continue;
			}
			if (line.starts_with("Compilation failed"))
			{

				CHROMA_CORE_ERROR("{}", line);
				continue;
			}
			const std::regex rgx(R"(\s*(.*)\s*\(\s*(\d+)\s*,\s*(\d+)\s*\)\s*:\s*(.*)\s*:\s*(.*))");
			if (!std::regex_match(line, rgx))
			{
				CHROMA_CORE_ERROR("{}", line);
				continue;
			}
			std::smatch m;
			std::regex_search(line, m, rgx);
			//CHROMA_CORE_TRACE("1. {}", m[1].str());
			//CHROMA_CORE_TRACE("2. {}", m[2].str());
			//CHROMA_CORE_TRACE("3. {}", m[3].str());
			//CHROMA_CORE_TRACE("4. {}", m[4].str());
			//CHROMA_CORE_TRACE("5. {}", m[5].str());
			//CHROMA_CORE_TRACE("");
			BuildMessage error;
			error.source = line;
			error.file = m[1].str();
			error.line = std::stoi(m[2].str());
			error.offset = std::stoi(m[3].str());
			error.error = m[4].str();
			if (error.error.starts_with("error"))
			{
				error.severity = BuildMessage::Severity::Error;
				size_t start = error.error.find("error ");
				error.error.erase(start, 6);
				Errors++;
			}
			else if (error.error.starts_with("warning"))
			{
				error.severity = BuildMessage::Severity::Warning;
				size_t start = error.error.find("warning ");
				error.error.erase(start, 8);
				Warnings++;
			}
			else
			{
				error.severity = BuildMessage::Severity::Message;
				Messages++;
			}
			error.message = m[5].str();
			BuildMessages.push_back(error);
			
		}

		if (!BuildMessages.empty())
		{
			ErrorWindow::Open = true;
		}

		if (Errors > 0)
		{
			BuildMessage* message = GetFirstOfType(BuildMessage::Severity::Error);
			if (message)
				EditorApp::SetInfoMessage(message->source, EditorApp::MessageSeverity::Error);
		}
		else if (Warnings > 0)
		{
			BuildMessage* message = GetFirstOfType(BuildMessage::Severity::Warning);
			if (message)
				EditorApp::SetInfoMessage(message->source, EditorApp::MessageSeverity::Warning);
		}
		else if (Messages > 0)
		{
			BuildMessage* message = GetFirstOfType(BuildMessage::Severity::Warning);
			if (message)
				EditorApp::SetInfoMessage(message->source, EditorApp::MessageSeverity::Warning);
		}
		else
		{
			EditorApp::SetInfoMessage("Finished", EditorApp::MessageSeverity::Info);
		}
			
	}

	bool Build::Success()
	{
		return _success;
	}
}