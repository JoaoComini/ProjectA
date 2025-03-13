#pragma once

#include "Resource/ResourceImporter.h"

namespace Engine
{
    class SceneImporter final : public ResourceImporter
    {
    public:
        void Import(const std::filesystem::path &source, const std::filesystem::path& destination) override;
        std::vector<std::string> GetImportExtensions() const override;
        std::string GetSaveExtension() const override;
    };
}