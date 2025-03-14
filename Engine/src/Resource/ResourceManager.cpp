#include "ResourceManager.h"

#include "Rendering/Renderer.h"

#include "Scripting/Script.h"

namespace Engine
{
    ResourceManager::ResourceManager(RenderContext& renderContext): device(renderContext.GetDevice())
    {
    }

    void ResourceManager::AddImporter(std::unique_ptr<ResourceImporter> importer)
    {
        importers.push_back(std::move(importer));
    }

    void ResourceManager::ImportResource(const std::filesystem::path& path)
    {
        const auto importer = GetImporterByExtension(path.extension());

        if (!importer)
        {
            return;
        }

        auto saveAs = importer->GetSaveExtension();

        auto destination = Project::GetImportsDirectory() / path.filename();
        destination.replace_extension(saveAs);

        const ResourceId id;

        importer->Import(id, path, destination);

        const ResourceMetadata metadata
        {
            .path = std::filesystem::relative(destination, Project::GetResourceDirectory()),
            .type = importer->GetResourceType()
        };

        ResourceRegistry::Get().ResourceCreated(id, metadata);
    }

    ResourceImporter* ResourceManager::GetImporterByExtension(const std::filesystem::path &extension) const
    {
        for (const auto& importer : importers)
        {
            const auto supported = importer->GetImportExtensions();

            if (std::ranges::find(supported.begin(), supported.end(), extension) != supported.end())
            {
                return importer.get();
            }
        }

        return nullptr;
    }

    bool ResourceManager::IsResourceLoaded(const ResourceId& id) const
    {
        return loadedResources.contains(id);
    }
};