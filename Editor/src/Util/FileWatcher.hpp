#pragma once

#include <filesystem>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>

using FileChangedFn = std::function<void(std::filesystem::path)>;

class FileWatcher
{
public:
    FileWatcher() = default;
    FileWatcher(std::filesystem::path path, std::chrono::duration<int, std::milli> delay = std::chrono::milliseconds(1000));
    ~FileWatcher();

    void OnUpdate(FileChangedFn onUpdateFn);

private:
    void Watch();

    std::filesystem::path path;
    std::chrono::duration<int, std::milli> delay;

    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> watched;

    std::atomic_bool running = false;
    std::thread thread;

    FileChangedFn onUpdateFn;
};