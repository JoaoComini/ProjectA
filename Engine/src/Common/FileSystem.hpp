#pragma once

#include <filesystem>

class FileSystem
{
public:
	static std::string ReadFile(std::filesystem::path path);
	static void WriteFile(std::filesystem::path path, std::string content);
};