#pragma once

#include "ResourceManager.hpp"

#include "Importer/GltfImporter.hpp"
#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/PrefabFactory.hpp"
#include "Resource/Factory/SceneFactory.hpp"

#include "yaml-cpp/yaml.h"

#include <fstream>

namespace Engine
{
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
    std::shared_ptr<Prefab> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        PrefabFactory factory;

        return factory.Load(path);
    }

    template<>
    std::shared_ptr<Scene> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        SceneFactory factory;

        return factory.Load(path);
    }

    template<>
    void ResourceManager::FactoryCreate<Texture>(std::filesystem::path path, TextureSpec& payload)
    {
        TextureFactory factory{ device };

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Mesh>(std::filesystem::path path, MeshSpec& payload)
    {
        MeshFactory factory{ device };

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Material>(std::filesystem::path path, MaterialSpec& payload)
    {
        MaterialFactory factory;

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Prefab>(std::filesystem::path path, Prefab& payload)
    {
        PrefabFactory factory;

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Scene>(std::filesystem::path path, Scene& payload)
    {
        SceneFactory factory;

        factory.Create(path, payload);
    }
};