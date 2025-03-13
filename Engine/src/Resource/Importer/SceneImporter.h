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
        virtual std::vector<std::string> GetImportExtensions() const = 0;
    };

    class SceneImporter final : public ResourceImporter
    {
    public:
        void Import(const std::filesystem::path &source, const std::filesystem::path& destination) override;
        std::vector<std::string> GetImportExtensions() const override;
        std::string GetSaveExtension() const override;

        void AddModule(std::unique_ptr<SceneImporterModule> module);
    private:
        SceneImporterModule* GetModuleByExtension(const std::filesystem::path &extension) const;

        std::vector<std::unique_ptr<SceneImporterModule>> modules;
    };
}