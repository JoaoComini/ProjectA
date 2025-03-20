#include "Project.h"

#include "Common/FileSystem.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	Project::Project(const std::filesystem::path &directory, ProjectConfig& config)
		: directory(directory), config(config)
	{
	}

	void Project::Load(const std::filesystem::path& path)
	{
		YAML::Node file = YAML::LoadFile(path.string());

		auto project = file["Project"];

		ProjectConfig config
		{
			.resourceDirectory = project["ResourceDirectory"].as<std::string>(),
			.resourceRegistry = project["ResourceRegistry"].as<std::string>()
		};

		activeProject = std::make_shared<Project>(path.parent_path(), config);

		if (const auto dir = GetResourceDirectory(); !FileSystem::Exists(dir))
		{
			FileSystem::CreateDirectory(dir);
		}

		if (const auto dir = GetImportsDirectory(); !FileSystem::Exists(dir))
		{
			FileSystem::CreateDirectory(dir);
		}
	}

	std::filesystem::path Project::GetProjectDirectory()
	{
		return activeProject->directory;
	}

	std::filesystem::path Project::GetResourceDirectory()
	{
		return GetProjectDirectory() / activeProject->config.resourceDirectory;
	}

	std::filesystem::path Project::GetImportsDirectory()
	{
		return GetResourceDirectory() / ".Imports";
	}

	std::filesystem::path Project::GetResourceRegistryPath()
	{
		return GetResourceDirectory() / activeProject->config.resourceRegistry;
	}
};