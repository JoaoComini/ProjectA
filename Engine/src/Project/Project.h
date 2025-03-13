#pragma once

namespace Engine
{
	struct ProjectConfig
	{
		std::filesystem::path resourceDirectory = "Resources";
		std::filesystem::path resourceRegistry = "registry.pareg";
	};

	class Project
	{
	public:
		Project(const std::filesystem::path &directory, ProjectConfig& config);

		static void Load(const std::filesystem::path& path = "");
		static std::filesystem::path GetProjectDirectory();
		static std::filesystem::path GetResourceDirectory();
		static std::filesystem::path GetImportsDirectory();
		static std::filesystem::path GetResourceRegistryPath();

	private:
		std::filesystem::path directory;
		ProjectConfig config;

		inline static std::shared_ptr<Project> activeProject;
	};


};