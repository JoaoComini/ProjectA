#pragma once

#include "Scene/Scene.h"
#include "Resource/ResourceImporter.h"

namespace Engine
{
    class SceneImporterModule
    {
    public:
        virtual ~SceneImporterModule() = default;
        virtual std::unique_ptr<Scene> Import(const std::filesystem::path& path) = 0;
        [[nodiscard]] virtual std::vector<std::string> GetImportExtensions() const = 0;
    };

    class SceneImporter final : public ResourceImporter
    {
    public:
        void Import(const std::filesystem::path& source, const std::filesystem::path& destination) override;
        [[nodiscard]] std::vector<std::string> GetImportExtensions() const override;
        [[nodiscard]] std::string GetSaveExtension() const override;
        [[nodiscard]] ResourceType GetResourceType() const override;

        void AddModule(std::unique_ptr<SceneImporterModule> module);

    private:
        [[nodiscard]] SceneImporterModule* GetModuleByExtension(const std::filesystem::path &extension) const;

        std::vector<std::unique_ptr<SceneImporterModule>> modules;
    };
}