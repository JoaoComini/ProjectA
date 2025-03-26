#pragma once

#include "Common/Singleton.h"

#include "Resource.h"
#include "ResourceImporter.h"
#include "ResourceRegistry.h"
#include "ResourceLoader.h"
#include "ResourceMetadata.h"
#include "ResourceSaver.h"

#include "Rendering/RenderContext.h"

#include "Project/Project.h"

namespace Engine
{
    class ResourceManager : public Singleton<ResourceManager>
    {
    public:
        explicit ResourceManager(RenderContext& renderContext);

        void ImportResource(const std::filesystem::path& path);
        void AddImporter(std::unique_ptr<ResourceImporter> importer);

        template<typename T>
        std::shared_ptr<T> LoadResource(const ResourceId& id)
        {
            if (! ResourceRegistry::Get().HasResource(id))
            {
                return nullptr;
            }

            if (IsResourceLoaded(id))
            {
                const auto& resource = loadedResources[id];

                return std::static_pointer_cast<T>(resource);
            }

            const auto mapping = ResourceRegistry::Get().FindMappingById(id);
            auto resourcePath = Project::GetResourceDirectory() / mapping->path;

            if (FileSystem::Exists(resourcePath.string() + ".metadata"))
            {
                ResourceMetadata metadata;
                metadata.LoadFromFile(resourcePath.string() + ".metadata");

                if (metadata.HasValue("Importer", "Destination"))
                {
                    resourcePath = metadata.GetValue<std::string>("Importer", "Destination");
                    resourcePath = Project::GetResourceDirectory() / resourcePath;
                }
            }

            auto resource =  ResourceLoader::Load<T>(resourcePath, device);
            resource->SetId(id);

            loadedResources[id] = resource;

            return resource;
        }

        template<typename T>
        ResourceId CreateResource(const std::filesystem::path& path, const T& resource)
        {
            const auto destination = Project::GetResourceDirectory() / path;

            ResourceSaver::Save<T>(destination, resource);

            if (auto id = ResourceRegistry::Get().FindResourceByPath(destination))
            {
                return id;
            }

            ResourceId id{};

            const ResourceMapping mapping
            {
                .path = path,
                .type = resource.GetType(),
            };

            ResourceRegistry::Get().ResourceCreated(id, mapping);

            return id;
        }

        template<typename T>
        void SaveResource(const ResourceId id, const T& resource)
        {
            if (!ResourceRegistry::Get().HasResource(id))
            {
                return;
            }

            const auto mapping = ResourceRegistry::Get().FindMappingById(id);

            ResourceSaver::Save<T>(Project::GetResourceDirectory() / mapping->path, resource);

            UnloadResource(id);
        }

        void UnloadResource(const ResourceId& id);
        void DeleteResource(const ResourceId& id);

    private:

        [[nodiscard]] ResourceImporter* GetImporterByExtension(const std::filesystem::path& extension) const;

        [[nodiscard]] bool IsResourceLoaded(const ResourceId& id) const;

        std::vector<std::unique_ptr<ResourceImporter>> importers;

        std::unordered_map<ResourceId, std::shared_ptr<Resource>> loadedResources;
        Vulkan::Device& device;
    };
};
