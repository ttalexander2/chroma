#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <readerwriterqueue.h>
#include <functional>

namespace Polychrome
{
	class FileWatcherThread
	{
	public:
		static void Init();
		static void SetWatch(const std::string& dir);

		static moodycamel::ReaderWriterQueue<std::function<void()>> file_queue;

		static std::thread file_watcher_thread;
		static std::atomic_bool file_watcher_thread_running;

	};
}