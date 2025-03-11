#pragma once

#include "Common/Singleton.hpp"

#include "Resource.hpp"
#include "ResourceImporter.hpp"
#include "ResourceRegistry.hpp"
#include "ResourceTools.hpp"
#include "Factory/SceneFactory.hpp"

#include "Rendering/RenderContext.hpp"

#include "Project/Project.hpp"

namespace Engine
{
    class ResourceManager: public Singleton<ResourceManager>
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

            auto resource = FactoryLoad<T>(Project::GetResourceDirectory() / metadata->path);
            resource->id = id;

            loadedResources[id] = resource;

            return resource;
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
            auto unique = ResourceTools::FindUniqueResourcePath(path, T::GetExtension());

            const ResourceId id{};

            FactoryCreate<T>(unique, payload);

            ResourceMetadata metadata
            {
                .path = std::filesystem::relative(unique, Project::GetResourceDirectory()),
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

            FactoryCreate<T>(Project::GetResourceDirectory() / metadata->path, payload);

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

        ResourceImporter* GetImporterByExtension(const std::filesystem::path& extension);

        [[nodiscard]] bool IsResourceLoaded(const ResourceId& id) const;

        template<typename T>
        std::shared_ptr<T> FactoryLoad(const std::filesystem::path& path);

        template<typename T, typename P>
        void FactoryCreate(const std::filesystem::path& path, P& payload);

        std::vector<std::unique_ptr<ResourceImporter>> importers;

        std::unordered_map<ResourceId, std::shared_ptr<Resource>> loadedResources;
        Vulkan::Device& device;
    };
};