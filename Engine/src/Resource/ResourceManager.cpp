#include "ResourceManager.h"

#include "ResourceMetadata.h"
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

        const auto saveAs = importer->GetSaveExtension();

        auto destination = Project::GetImportsDirectory() / path.filename();
        destination.replace_extension(saveAs);

        importer->Import(path, destination);

        ResourceMetadata metadata;
        metadata.SetValue("Importer", "Destination", std::filesystem::relative(destination, Project::GetResourceDirectory()).string());
        metadata.SaveToFile(path.string() + ".metadata");

        const ResourceMapping mapping
        {
            .path = std::filesystem::relative(path, Project::GetResourceDirectory()),
            .type = importer->GetResourceType()
        };

        ResourceRegistry::Get().ResourceCreated(ResourceId{}, mapping);
    }

    void ResourceManager::UnloadResource(const ResourceId &id)
    {
        if (! ResourceRegistry::Get().HasResource(id))
        {
            return;
        }

        if (! IsResourceLoaded(id))
        {
            return;
        }

        loadedResources.erase(id);
    }

    void ResourceManager::DeleteResource(const ResourceId& id)
    {
        if (!ResourceRegistry::Get().HasResource(id))
        {
            return;
        }

        const auto mapping = ResourceRegistry::Get().FindMappingById(id);

        std::filesystem::remove(Project::GetResourceDirectory() / mapping->path);

        device.WaitIdle();

        if (IsResourceLoaded(id))
        {
            loadedResources.erase(id);
        }

        ResourceRegistry::Get().ResourceDeleted(id);
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