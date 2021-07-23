#pragma once

#include "FileWatcher.h"
#include <readerwriterqueue.h>

namespace Chroma
{
	class AsyncFileWatcher
	{
	public:
		void MainThreadHandle();
		void Run();
		void Stop();

	private:
		moodycamel::ReaderWriterQueue<std::function<void()>> file_queue;
		std::thread file_watcher_thread;
		std::atomic_bool file_watcher_thread_running;
	};
}