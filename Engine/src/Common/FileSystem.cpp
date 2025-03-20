#include "FileSystem.h"

std::string FileSystem::ReadFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    std::string buffer;

    file.seekg(0, std::ios::end);
    auto length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer.resize(length);
    file.read(&buffer[0], length);

    return buffer;
}

void FileSystem::WriteFile(const std::filesystem::path& path, const std::string &content)
{
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);

    file << content;
}

bool FileSystem::Exists(const std::filesystem::path &path)
{
    return std::filesystem::exists(path);
}

bool FileSystem::CreateDirectory(const std::filesystem::path &path)
{
    return std::filesystem::create_directory(path);
}
