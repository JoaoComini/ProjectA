#pragma once

#include "ResourceManager.hpp"

#include "Importer/GltfImporter.hpp"
#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/ModelFactory.hpp"

namespace Engine
{
    void ResourceManager::Setup(const Vulkan::Device& device)
    {
        if (!instance)
        {
            instance = new ResourceManager(device);
        }
    }

    ResourceManager::ResourceManager(const Vulkan::Device& device)
        : device(device)
    {
    }

    void ResourceManager::ImportResource(const std::filesystem::path& path)
    {
        GltfImporter importer{ device };
        
        importer.Import(path);
    }

    bool ResourceManager::IsResourceLoaded(const ResourceId& id)
    {
        return loadedResources.find(id) != loadedResources.end();
    }

    bool ResourceManager::IsResourceValid(const ResourceId& id)
    {
        return resourceRegistry.find(id) != resourceRegistry.end();
    }

    void ResourceManager::OnResourceImport(std::shared_ptr<Resource> resource, const ResourceMetadata& metadata)
    {
        resourceRegistry.emplace(resource->GetId(), metadata);
        loadedResources.emplace(resource->GetId(), resource);
    }

    template<>
    std::shared_ptr<Texture> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        TextureFactory factory{ device };

        return factory.Load(path);
    }

    template<>
    std::shared_ptr<Mesh> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        MeshFactory factory{ device };

        return factory.Load(path);
    }

    template<>
    std::shared_ptr<Material> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        MaterialFactory factory;

        return factory.Load(path);
    }

    template<>
    std::shared_ptr<Model> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        ModelFactory factory;

        return factory.Load(path);
    }
};