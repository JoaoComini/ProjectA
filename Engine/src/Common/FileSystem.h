#pragma once

class FileSystem
{
public:
	static std::string ReadFile(const std::filesystem::path& path);
	static void WriteFile(const std::filesystem::path& path, const std::string& content);
	static bool Exists(const std::filesystem::path& path);
	static bool CreateDirectory(const std::filesystem::path& path);
};