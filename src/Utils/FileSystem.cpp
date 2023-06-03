#include "FileSystem.hpp"

#include <fstream>


std::string FileSystem::ReadFile(std::string path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}
