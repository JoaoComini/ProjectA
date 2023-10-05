#pragma once

#include "Common/Singleton.hpp"

#include "Resource/Resource.hpp"

#include "Vulkan/Device.hpp"

#include <unordered_map>

namespace Engine
{
    class ResourceManager: public Singleton<ResourceManager>
    {
    public:
        static void Setup(const Vulkan::Device& device);

        template<typename T>
        std::shared_ptr<T> LoadResource(const ResourceId& id)
        {
            if (!IsResourceValid(id))
            {
                return nullptr;
            }

            if (IsResourceLoaded(id))
            {
                auto resource = loadedResources.at(id);

                return std::static_pointer_cast<T>(resource);
            }

            const auto& metadata = resourceRegistry.at(id);

            return FactoryLoad<T>(metadata.path);
        }

        template<typename T>
        std::shared_ptr<T> FactoryLoad(std::filesystem::path path);

        void ImportResource(const std::filesystem::path& path);

        void OnResourceImport(std::shared_ptr<Resource> resource, const ResourceMetadata& metadata);

        std::vector<ResourceId> FindResourcesOfType(ResourceType type)
        {
            std::vector<ResourceId> resources;

            for (auto it = resourceRegistry.begin(); it != resourceRegistry.end(); it++)
            {
                if (it->second.type == type)
                {
                    resources.push_back(it->first);
                }
            }

            return resources;
        }

    private:
        ResourceManager(const Vulkan::Device& device);

        bool IsResourceValid(const ResourceId& id);
        bool IsResourceLoaded(const ResourceId& id);

        std::unordered_map<ResourceId, ResourceMetadata> resourceRegistry;
        std::unordered_map<ResourceId, std::shared_ptr<Resource>> loadedResources;

        const Vulkan::Device& device;
    };
};