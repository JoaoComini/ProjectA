#pragma once

#include "Common/Singleton.hpp"

#include "Resource/Resource.hpp"
#include "Resource/ResourceRegistry.hpp"

#include "Vulkan/Device.hpp"

#include "Project/Project.hpp"

#include <unordered_map>

namespace Engine
{
    class ResourceManager: public Singleton<ResourceManager>
    {
    public:
        ResourceManager(const Vulkan::Device& device);

        template<typename T>
        std::shared_ptr<T> LoadResource(const ResourceId& id)
        {
            if (!ResourceRegistry::Get().HasResource(id))
            {
                return nullptr;
            }

            if (IsResourceLoaded(id))
            {
                auto resource = loadedResources[id];

                return std::static_pointer_cast<T>(resource);
            }

            const auto metadata = ResourceRegistry::Get().FindMetadataById(id);

            auto resource = FactoryLoad<T>(Project::GetResourceDirectory() / metadata->path);

            loadedResources[id] = resource;

            return resource;
        }

        void ImportResource(const std::filesystem::path& path);
    private:

        bool IsResourceLoaded(const ResourceId& id);

        template<typename T>
        std::shared_ptr<T> FactoryLoad(std::filesystem::path path);

        std::unordered_map<ResourceId, std::shared_ptr<Resource>> loadedResources;
        const Vulkan::Device& device;
    };
};