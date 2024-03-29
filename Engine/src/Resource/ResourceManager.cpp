#pragma once

#include "ResourceManager.hpp"

#include "Rendering/Cubemap.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderContext.hpp"

#include "Importer/GltfImporter.hpp"
#include "Importer/TextureImporter.hpp"
#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/PrefabFactory.hpp"
#include "Resource/Factory/SceneFactory.hpp"


namespace Engine
{
    ResourceManager::ResourceManager() : device(Renderer::Get().GetRenderContext().GetDevice())
    {
    }

    void ResourceManager::ImportResource(const std::filesystem::path& path)
    {
        auto extension = path.extension();

        if (extension == ".glb")
        {
            GltfImporter importer{ device };

            importer.Import(path);
        }
        else if (extension == ".hdr")
        {
            TextureImporter importer{ device };

            importer.ImportCubemap(path);
        }
        else
        {
            throw std::runtime_error("resource format not supported");
        }
    }

    bool ResourceManager::IsResourceLoaded(const ResourceId& id)
    {
        return loadedResources.find(id) != loadedResources.end();
    }

    template<>
    std::shared_ptr<Texture> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        TextureFactory factory{ device };

        auto texture = factory.Load(path);
        texture->CreateVulkanResources(device);
        texture->UploadDataToGpu(device);

        return texture;
    }

    template<>
    std::shared_ptr<Cubemap> ResourceManager::FactoryLoad(std::filesystem::path path)
    {
        TextureFactory factory{ device };

        auto texture = factory.Load(path);

        auto cubemap = std::make_shared<Cubemap>(std::move(*texture));
        cubemap->CreateVulkanResources(device);
        cubemap->UploadDataToGpu(device);

        return cubemap;
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
    void ResourceManager::FactoryCreate<Texture>(std::filesystem::path path, Texture& payload)
    {
        TextureFactory factory{ device };

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Cubemap>(std::filesystem::path path, Cubemap& payload)
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