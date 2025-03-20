#pragma once

#include <utility>
#include <variant>

#include "Resource.h"

namespace Engine
{
    class ResourceImporter
    {
    public:
        virtual ~ResourceImporter() = default;
        virtual void Import(const std::filesystem::path& source, const std::filesystem::path& destination) = 0;
        [[nodiscard]] virtual std::vector<std::string> GetImportExtensions() const = 0;
        [[nodiscard]] virtual std::string GetSaveExtension() const = 0;
        [[nodiscard]] virtual ResourceType GetResourceType() const = 0;
    };

}
