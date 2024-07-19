#pragma once

#include "Project/Project.hpp"

#include <filesystem>

namespace Engine::ResourceTools
{
    static std::filesystem::path FindUniqueResourcePath(std::filesystem::path path, std::string extension)
    {
        auto base = Project::GetResourceDirectory() / path;

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

            existing = ResourceRegistry::Get().FindResourceByPath(std::filesystem::relative(unique, Project::GetResourceDirectory()));
        } while (existing);

        return unique;
    }
};