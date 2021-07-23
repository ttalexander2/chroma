#include "chromapch.h"
#include "AsyncFileWatcher.h"

void Chroma::AsyncFileWatcher::MainThreadHandle()
{
	std::function<void()> func;
	while (true)
	{
		bool success = file_queue.try_dequeue(func);
		if (success)
			func();
		else
			break;
	}

}

void Chroma::AsyncFileWatcher::Run()
{
	file_watcher_thread_running.store(true);
}

void Chroma::AsyncFileWatcher::Stop()
{
	file_watcher_thread_running.store(false);
}
