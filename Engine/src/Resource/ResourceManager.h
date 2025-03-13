#pragma once

#include "Common/Singleton.h"

#include "Resource.h"
#include "ResourceImporter.h"
#include "ResourceRegistry.h"

#include "Rendering/RenderContext.h"

#include "Project/Project.h"

namespace Engine
{
    class ResourceManager : public Singleton<ResourceManager>
    {
    public:
        explicit ResourceManager(RenderContext& renderContext);

        void AddImporter(std::unique_ptr<ResourceImporter> importer);

        template<typename T>
        std::shared_ptr<T> LoadResource(const ResourceId& id)
        {
            if (!ResourceRegistry::Get().HasResource(id))
            {
                return nullptr;
            }

            if (IsResourceLoaded(id))
            {
                const auto& resource = loadedResources[id];

                return std::static_pointer_cast<T>(resource);
            }

            const auto metadata = ResourceRegistry::Get().FindMetadataById(id);

            // auto resource = FactoryLoad<T>(Project::GetResourceDirectory() / metadata->path);
            // resource->id = id;
            //
            // loadedResources[id] = resource;
            //
            // return resource;

            return nullptr;
        }

        void UnloadResource(const ResourceId& id)
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

        template<typename T, typename P>
        ResourceId CreateResource(std::filesystem::path path, P& payload)
        {
            auto destination = path.replace_extension(T::GetExtension());

            const ResourceId id{};

            // FactoryCreate<T>(destination, payload);

            const ResourceMetadata metadata
            {
                .path = std::filesystem::relative(destination, Project::GetResourceDirectory()),
                .type = T::GetStaticType()
            };

            ResourceRegistry::Get().ResourceCreated(id, metadata);

            return id;
        }

        template<typename T, typename P>
        void SaveResource(const ResourceId id, P& payload)
        {
            if (!ResourceRegistry::Get().HasResource(id))
            {
                return;
            }

            const auto metadata = ResourceRegistry::Get().FindMetadataById(id);

            // FactoryCreate<T>(Project::GetResourceDirectory() / metadata->path, payload);

            if (IsResourceLoaded(id))
            {
                loadedResources.erase(id);
            }
        }

        void DeleteResource(const ResourceId id)
        {
            if (!ResourceRegistry::Get().HasResource(id))
            {
                return;
            }

            const auto metadata = ResourceRegistry::Get().FindMetadataById(id);

            std::filesystem::remove(Project::GetResourceDirectory() / metadata->path);

            device.WaitIdle();

            if (IsResourceLoaded(id))
            {
                loadedResources.erase(id);
            }

            ResourceRegistry::Get().ResourceDeleted(id);
        }

        void ImportResource(const std::filesystem::path& path);
    private:

        ResourceImporter* GetImporterByExtension(const std::filesystem::path& extension) const;

        [[nodiscard]] bool IsResourceLoaded(const ResourceId& id) const;

        std::vector<std::unique_ptr<ResourceImporter>> importers;

        std::unordered_map<ResourceId, std::shared_ptr<Resource>> loadedResources;
        Vulkan::Device& device;
    };
};