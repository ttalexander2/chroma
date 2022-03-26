#pragma once

#include <string>
#include <atomic>
#include <mutex>
#include <Chroma.h>


namespace Polychrome
{
	class EditorApp;

	class Launcher
	{

		friend class EditorApp;

	public:

		struct RecentProjectInfo
		{
			std::string Name;
			std::string TimeStamp;
			std::string Path;
			bool Pinned = false;
		};

		Launcher();
		~Launcher();

		void Init();
		void ImGuiDraw(Chroma::Time time, float width, float height, void* nativeWindowHandle);



		static std::atomic_bool Loading;
		static std::atomic_bool Done;
		static std::atomic_bool Stopped;
		static std::mutex ProgressLock;
		static std::string ProgressMessage; //Used by multiple threads (use mutex first)
		static float Progress;  //Used by multiple threads (use mutex first)

		static std::string path;
		static std::string project_name;
		static bool create;
		static std::string starting_scene;
	};
}