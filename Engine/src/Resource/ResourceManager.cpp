#pragma once

#include "ResourceManager.hpp"

#include "Importer/GltfImporter.hpp"
#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/ModelFactory.hpp"

#include "yaml-cpp/yaml.h"

#include <fstream>

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

        SerializeRegistry();
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

    std::unordered_map<ResourceId, ResourceMetadata>& ResourceManager::GetResourceRegistry()
    {
        return resourceRegistry;
    }

    void ResourceManager::SerializeRegistry()
    {
        auto path = Project::GetResourceRegistryPath();

        YAML::Emitter out;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "ResourceRegistry" << YAML::Value;

            out << YAML::BeginSeq;
            for (const auto& [id, metadata] : resourceRegistry)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "Id" << YAML::Value << static_cast<uint64_t>(id);
                out << YAML::Key << "Path" << YAML::Value << metadata.path.generic_string();
                out << YAML::Key << "Type" << YAML::Value << ResourceTypeToString(metadata.type);
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }

        std::ofstream fout(path);
        fout << out.c_str();
    }

    void ResourceManager::DeserializeRegistry()
    {
        auto path = Project::GetResourceRegistryPath();
        YAML::Node node;
        try
        {
            node = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            return;
        }

        auto root = node["ResourceRegistry"];
        if (!root)
        {
            return;
        }

        for (const auto& node : root)
        {
            Uuid id = node["Id"].as<uint64_t>();

            auto& metadata = resourceRegistry[id];

            metadata.path = node["Path"].as<std::string>();
            metadata.type = StringToResourceType(node["Type"].as<std::string>());
        }
    }
};