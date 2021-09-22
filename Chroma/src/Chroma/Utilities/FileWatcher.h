#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

// Define available file changes
enum class FileStatus {created, modified, erased};

class FileWatcher {
public:
    std::string path_to_watch;
    // Time interval at which we check the base folder for changes
    std::chrono::milliseconds delay;

    // Keep a record of files from the base directory and their last modification time
    FileWatcher(std::string path_to_watch, std::chrono::milliseconds delay) : path_to_watch{path_to_watch}, delay{delay} {
        for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
            paths_[file.path().string()] = std::filesystem::last_write_time(file);
        }
    }

    // Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
    void start(std::atomic_bool& running, const std::function<void (const std::string&, const std::string&, FileStatus)> &action) {

        std::chrono::high_resolution_clock sc;
        auto start = sc.now();

        while(running.load()) {

            //Custom timer to check for durration. This way the atomic_bool is checked every loop, and the thread will exit gracefully when shut down.
            auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(sc.now() - start);
            if (time_span < delay)
                continue;
            start = sc.now();

            auto it = paths_.begin();
            while (it != paths_.end()) {
                if (!std::filesystem::exists(it->first)) {
                    action(it->first, path_to_watch, FileStatus::erased);
                    it = paths_.erase(it);
                }
                else {
                    it++;
                }                    
            }

            // Check if a file was created or modified
            for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
                auto current_file_last_write_time = std::filesystem::last_write_time(file);

                // File creation
                if(!contains(file.path().string())) {
                    paths_[file.path().string()] = current_file_last_write_time;
                    action(file.path().string(), path_to_watch, FileStatus::created);
                // File modification
                } else {
                    if(paths_[file.path().string()] != current_file_last_write_time) {
                        paths_[file.path().string()] = current_file_last_write_time;
                        action(file.path().string(), path_to_watch, FileStatus::modified);
                    }
                }
            }
        }
    }


private:
    std::unordered_map<std::string, std::filesystem::file_time_type> paths_;

    // Check if "paths_" contains a given key
    // If your compiler supports C++20 use paths_.contains(key) instead of this function
    bool contains(const std::string &key) {
        auto el = paths_.find(key);
        return el != paths_.end();
    }
};
