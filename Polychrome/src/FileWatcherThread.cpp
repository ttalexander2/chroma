#include "FileWatcherThread.h"
#include <filesystem>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Utilities/FileWatcher.h>
#include "EditorApp.h"
#include <readerwriterqueue.h>
#include <Chroma/Scripting/MonoScripting.h>
#include "Project.h"
#include <Chroma/Scripting/ScriptEngineRegistry.h>
#include "Build.h"
#include <Chroma/IO/FileSystem.h>


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
						else if (extension == ".cs")
						{
							auto result = Build::BuildMonoAssembly(root, Project::Name);
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
						else if (extension == ".cs")
						{
							auto result = Build::BuildMonoAssembly(root, Project::Name);
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
						else if (extension == ".cs")
						{
							auto result = Build::BuildMonoAssembly(root, Project::Name);
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


