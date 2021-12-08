#include "Build.h"
#include <filesystem>
#include <Chroma/Scripting/MonoScripting.h>
#include <regex>
#include "ErrorWindow.h"
#include "EditorApp.h"

namespace Polychrome
{
	std::vector<Build::BuildMessage> Build::BuildMessages = std::vector<Build::BuildMessage>();
	size_t Build::Errors = 0;
	size_t Build::Warnings = 0;
	size_t Build::Messages = 0;

	static bool _success = false;

	//TODO: Build on a separate thread. Reload on dll change.
	bool Build::BuildMonoAssembly(const std::string& path, const std::string& name)
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
				monoLibStr += " -r:" + p.path().string();
			}
		}

		std::string logFile = path + "\\bin\\" + "build.log";
		int success = system((".\\mono\\bin\\mono.exe mono\\lib\\mono\\4.5\\mcs.exe -debug -target:library -nostdlib -out:\"" + std::filesystem::absolute(path).string() + "\\bin\\" + name + ".dll\" -r:Chroma.Mono.dll" + monoLibStr + " -recurse:\"" + std::filesystem::absolute(path).string() + "\\**.cs\" 2> " + logFile + " 1>&2").c_str());

		std::ifstream stream = std::ifstream(logFile);
		ParseErrors(stream);
		stream.close();

		_success = (success == 0);


		if (std::filesystem::exists(path + "\\bin\\" + name + ".dll"))
			return Chroma::MonoScripting::LoadAppAssembly(path + "\\bin\\" + name + ".dll") && success == 0;

		return success == 0;
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