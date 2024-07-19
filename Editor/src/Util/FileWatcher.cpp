#include "FileWatcher.hpp"


FileWatcher::FileWatcher(std::filesystem::path path, std::chrono::duration<int, std::milli> delay)
    : path(path), delay(delay), running(true)
{
    for (auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
        watched[entry.path()] = std::filesystem::last_write_time(entry);
    }

    thread = std::thread{ &FileWatcher::Watch, this };
}

FileWatcher::~FileWatcher()
{
    running = false;
    thread.join();
}

void FileWatcher::Watch()
{
    while (running)
    {
        std::this_thread::sleep_for(delay);

        for (auto it = watched.begin(); it != watched.end(); it++)
        {
            if (! std::filesystem::exists(it->first))
            {
                // TODO: deleted. deal with it later...
                continue;
            }

            auto last = std::filesystem::last_write_time(it->first);

            if (it->second == last)
            {
                continue;
            }

            it->second = last;

            onUpdateFn(std::filesystem::relative(it->first, path));
        }
    }
}

void FileWatcher::OnUpdate(FileChangedFn onUpdateFn)
{
    this->onUpdateFn = onUpdateFn;
}
