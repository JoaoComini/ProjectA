#pragma once

#include "Project/Project.hpp"

namespace Engine::ResourceTools
{
    static std::filesystem::path FindUniqueResourcePath(const std::filesystem::path& path, const std::string& extension)
    {
        const auto base = Project::GetResourceDirectory() / path;

        std::filesystem::path unique;
        ResourceId existing;
        auto current = 0;
        do
        {
            unique = base;

            if (current > 0)
            {
                unique += "_" + std::to_string(current);
            }

            unique.replace_extension("." + extension);
            current += 1;

            existing = ResourceRegistry::Get().FindResourceByPath(relative(unique, Project::GetResourceDirectory()));
        } while (existing);

        return unique;
    }

    static std::filesystem::path FindUniqueResourceDirectory(const std::filesystem::path& path)
    {
        const auto base = Project::GetResourceDirectory() / path.stem();

        std::filesystem::path directory;
        auto current = 0;
        do
        {
            directory = base;

            if (current > 0)
            {
                directory += "_" + std::to_string(current);
            }

            current += 1;
        } while (exists(directory));

        return directory;
    }
};