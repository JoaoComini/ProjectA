#include "ResourceManager.h"

#include "Rendering/Cubemap.h"
#include "Rendering/Renderer.h"

#include "Importer/GltfImporter.h"
#include "Importer/TextureImporter.h"
#include "Resource/Factory/TextureFactory.h"
#include "Resource/Factory/MeshFactory.h"
#include "Resource/Factory/MaterialFactory.h"
#include "Resource/Factory/SceneFactory.h"

#include "Scripting/Script.h"

#include "Common/FileSystem.h"

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

        auto saveAs = importer->GetSaveExtension();

        auto destination = Project::GetImportsDirectory() / path;
        destination.replace_extension(saveAs);

        importer->Import(path, destination);
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

    template<>
    std::shared_ptr<Texture> ResourceManager::FactoryLoad(const std::filesystem::path& path)
    {
        TextureFactory factory{ device };

        auto texture = factory.Load(path);
        texture->UploadToGpu(device);

        return texture;
    }

    template<>
    std::shared_ptr<Cubemap> ResourceManager::FactoryLoad(const std::filesystem::path& path)
    {
        TextureFactory factory{ device };

        const auto texture = factory.Load(path);

        auto cubemap = std::make_shared<Cubemap>(std::move(*texture));
        cubemap->UploadToGpu(device);

        return cubemap;
    }

    template<>
    std::shared_ptr<Mesh> ResourceManager::FactoryLoad(const std::filesystem::path& path)
    {
        MeshFactory factory{ device };

        return factory.Load(path);
    }

    template<>
    std::shared_ptr<Material> ResourceManager::FactoryLoad(const std::filesystem::path& path)
    {
        MaterialFactory factory;

        return factory.Load(path);
    }

    template<>
    std::shared_ptr<Scene> ResourceManager::FactoryLoad(const std::filesystem::path& path)
    {
        SceneFactory factory;

        return factory.Load(path);
    }
    template<>
    std::shared_ptr<Script> ResourceManager::FactoryLoad(const std::filesystem::path& path)
    {
        auto code = FileSystem::ReadFile(path);

        return std::make_shared<Script>(code);
    }


    template<>
    void ResourceManager::FactoryCreate<Texture>(const std::filesystem::path& path, Texture& payload)
    {
        TextureFactory factory{ device };

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Cubemap>(const std::filesystem::path& path, Cubemap& payload)
    {
        TextureFactory factory{ device };

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Mesh>(const std::filesystem::path& path, MeshSpec& payload)
    {
        MeshFactory factory{ device };

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Material>(const std::filesystem::path& path, MaterialSpec& payload)
    {
        MaterialFactory factory;

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Scene>(const std::filesystem::path& path, Scene& payload)
    {
        SceneFactory factory;

        factory.Create(path, payload);
    }

    template<>
    void ResourceManager::FactoryCreate<Script>(const std::filesystem::path& path, Script& payload)
    {
        FileSystem::WriteFile(path, std::string{ payload.GetCode() });
    }
};