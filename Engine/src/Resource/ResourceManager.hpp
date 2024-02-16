#pragma once

#include "Common/Singleton.hpp"

#include "Resource.hpp"
#include "ResourceRegistry.hpp"
#include "ResourceTools.hpp"
#include "Factory/Factory.hpp"
#include "Factory/SceneFactory.hpp"

#include "Vulkan/Device.hpp"

#include "Project/Project.hpp"

#include <unordered_map>

namespace Engine
{
    class ResourceManager: public Singleton<ResourceManager>
    {
    public:
        ResourceManager();

        template<typename T>
        std::shared_ptr<T> LoadResource(const ResourceId& id)
        {
            if (!ResourceRegistry::Get().HasResource(id))
            {
                return nullptr;
            }

            if (IsResourceLoaded(id))
            {
                auto& resource = loadedResources[id];

                return std::static_pointer_cast<T>(resource);
            }

            const auto metadata = ResourceRegistry::Get().FindMetadataById(id);

            auto resource = FactoryLoad<T>(Project::GetResourceDirectory() / metadata->path);
            resource->id = id;

            loadedResources[id] = resource;

            return resource;
        }

        template<typename T, typename P>
        ResourceId CreateResource(std::filesystem::path path, P& payload)
        {
            auto unique = ResourceTools::FindUniqueResourcePath(path);

            ResourceId id{};

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
        void SaveResource(ResourceId id, P& payload)
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

        void DeleteResource(ResourceId id)
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

        bool IsResourceLoaded(const ResourceId& id);

        template<typename T>
        std::shared_ptr<T> FactoryLoad(std::filesystem::path path);

        template<typename T, typename P>
        void FactoryCreate(std::filesystem::path path, P& payload);

        std::unordered_map<ResourceId, std::shared_ptr<Resource>> loadedResources;
        Vulkan::Device& device;
    };
};