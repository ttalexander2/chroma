#include "FileWatcherThread.h"
#include <filesystem>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Utilities/FileWatcher.h>
#include "EditorApp.h"
#include <readerwriterqueue.h>
#include <Chroma/Scripting/MonoScripting.h>
#include "Project.h"
#include <Chroma/Scripting/ScriptEngineRegistry.h>


namespace Polychrome
{
	moodycamel::ReaderWriterQueue<std::function<void()>> FileWatcherThread::file_queue;

	std::thread FileWatcherThread::file_watcher_thread;
	std::atomic_bool FileWatcherThread::file_watcher_thread_running;

	void Polychrome::FileWatcherThread::Init()
	{

	}

	void Polychrome::FileWatcherThread::SetWatch(const std::string& asset_dir)
	{
		//CHROMA_CORE_INFO("{}", asset_dir);
		std::string watch_dir = std::filesystem::path(asset_dir).parent_path().string();
		//CHROMA_CORE_INFO("{}", watch_dir);
		// This needs to be replaced
		for (auto& file : std::filesystem::recursive_directory_iterator(watch_dir))
		{
			if (file.is_regular_file())
			{
				//CHROMA_CORE_INFO("{}", file.path().string());
				std::string extension = file.path().extension().string();
				if (extension == ".ase" || extension == ".aseprite" || extension == ".png" || extension == ".jpg")
				{
					Chroma::AssetManager::LoadSprite(file.path().lexically_relative(asset_dir).string());
				}
				else if (extension == ".lua")
				{
					Chroma::LuaScripting::Scripts.push_back(file.path().lexically_relative(asset_dir).string());
				}
			}
		}

		auto result = Chroma::MonoScripting::BuildAssembly(watch_dir, Project::Name);
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



		file_watcher_thread_running.store(true);

		file_watcher_thread = std::thread([](const std::string& directory) {
			FileWatcher fw{ directory, std::chrono::milliseconds(3000) };

			fw.start(file_watcher_thread_running, [&](const std::string& file, const std::string& dr, FileStatus status) {
				// Process only regular files, all other file types are ignored
				std::string root = dr;
				if (!std::filesystem::is_regular_file(std::filesystem::path(file)) && status != FileStatus::erased)
				{
					return;
				}

				switch (status)
				{
				case FileStatus::created:
					file_queue.enqueue([file, root]() {
						std::string relative = std::filesystem::path(file).parent_path().lexically_relative(root).string();
						//CHROMA_CORE_TRACE("File Created: {}", relative);
						std::string extension = std::filesystem::path(file).extension().string();
						if (extension == ".ase" || extension == ".aseprite" || extension == ".png" || extension == ".jpg")
						{
							Chroma::AssetManager::LoadSprite(relative);
						}
						else if (extension == ".lua")
						{
							Chroma::LuaScripting::Scripts.push_back(relative);
						}
						else if (extension == ".cs")
						{
							auto result = Chroma::MonoScripting::BuildAssembly(root, Project::Name);
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
						});
					break;
				case FileStatus::modified:
					file_queue.enqueue([file, root]() {
						std::string extension = std::filesystem::path(file).extension().string();
						std::string relative = std::filesystem::path(file).parent_path().lexically_relative(root).string();
						//CHROMA_CORE_TRACE("File Modified: {}", relative);
						if (extension == ".ase" || extension == ".aseprite" || extension == ".png" || extension == ".jpg")
						{
							if (Chroma::AssetManager::HasSprite(relative))
							{
								Chroma::AssetManager::GetSprite(relative)->Reload();
							}
							else
							{
								Chroma::AssetManager::LoadSprite(relative);
							}
						}
						else if (extension == ".lua")
						{
							for (auto entity : EditorApp::CurrentScene->Registry.view<Chroma::LuaScript>())
							{
								auto& script = EditorApp::CurrentScene->GetComponent<Chroma::LuaScript>(entity);
								if (std::filesystem::equivalent(script.Path, relative))
								{
									script.ReloadState();
									script.Success = Chroma::LuaScripting::LoadScriptFromFile(script.Path, script.Environment);
									script.ReloadCoroutines();
								}
							}
						}
						else if (extension == ".cs")
						{
							auto result = Chroma::MonoScripting::BuildAssembly(root, Project::Name);
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
					});
					break;
				case FileStatus::erased:
					file_queue.enqueue([file, root]() {
						std::string relative = std::filesystem::path(file).parent_path().lexically_relative(root).string();
						//CHROMA_CORE_TRACE("File Erased: {}", relative);
						std::string extension = std::filesystem::path(relative).extension().string();
						if (extension == ".ase" || extension == ".png" || extension == ".jpg")
						{

						}
						else if (extension == ".lua")
						{
							auto it = std::find(Chroma::LuaScripting::Scripts.begin(), Chroma::LuaScripting::Scripts.end(), relative);
							Chroma::LuaScripting::Scripts.erase(it);
						}
						else if (extension == ".cs")
						{
							auto result = Chroma::MonoScripting::BuildAssembly(root, Project::Name);
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
						});

					break;
				default:
					CHROMA_CORE_TRACE("Error! Unknown file status.\n");
				}
			});
		}, watch_dir);


	}
}


