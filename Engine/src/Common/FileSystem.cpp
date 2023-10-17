#include "FileSystem.hpp"

#include <fstream>
#include <sstream>

std::string FileSystem::ReadFile(std::filesystem::path path)
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

void FileSystem::WriteFile(std::filesystem::path path, std::string content)
{
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);

    file << content;
}
