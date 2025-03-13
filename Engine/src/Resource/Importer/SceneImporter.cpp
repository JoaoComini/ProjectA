#include "SceneImporter.h"


namespace Engine
{
    void SceneImporter::Import(const std::filesystem::path &source, const std::filesystem::path &destination)
    {
        auto module = GetModuleByExtension(source.extension());

        if (!module)
        {
            return;
        }

        auto scene = module->Import(source);

        // TODO: serialize scene (create other resources: meshes, materials, etc?)
    }

    std::vector<std::string> SceneImporter::GetImportExtensions() const
    {
        std::vector<std::string> extensions;

        for (const auto& module : modules)
        {
            auto supported = module->GetImportExtensions();

            extensions.insert(extensions.end(), supported.begin(), supported.end());
        }

        return extensions;
    }

    std::string SceneImporter::GetSaveExtension() const
    {
        return ".scene";
    }

    void SceneImporter::AddModule(std::unique_ptr<SceneImporterModule> module)
    {
        modules.push_back(std::move(module));
    }

    SceneImporterModule* SceneImporter::GetModuleByExtension(const std::filesystem::path &extension) const
    {
        for (const auto& module : modules)
        {
            const auto supported = module->GetImportExtensions();

            if (std::ranges::find(supported.begin(), supported.end(), extension) != supported.end())
            {
                return module.get();
            }
        }

        return nullptr;
    }
}
