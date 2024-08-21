#include "ResourceRegistry.hpp"

#include "Project/Project.hpp"

#include <yaml-cpp/yaml.h>

namespace Engine
{
    bool ResourceRegistry::HasResource(ResourceId id)
    {
        return registry.find(id) != registry.end();
    }

    bool ResourceRegistry::HasResourceOnPath(std::filesystem::path path)
    {
        return resourcesByPath.find(path) != resourcesByPath.end();
    }

    ResourceMetadata* ResourceRegistry::FindMetadataById(ResourceId id)
    {
        if (HasResource(id))
        {
            return &registry[id];
        }

        return nullptr;
    }

    ResourceId ResourceRegistry::FindResourceByPath(std::filesystem::path path)
    {
        if (HasResourceOnPath(path))
        {
            return resourcesByPath[path];
        }

        return { 0 };
    }

    std::vector<ResourceEntry> ResourceRegistry::GetEntriesByType(ResourceType type)
    {
        std::vector<ResourceEntry> resources;

        for (const auto& [id, metadata] : registry)
        {
            if (metadata.type != type)
            {
                continue;
            }

            resources.push_back({id, metadata});
        }

        return resources;
    }

    void ResourceRegistry::ResourceCreated(ResourceId id, ResourceMetadata metadata)
    {
        registry[id] = metadata;
        resourcesByPath[metadata.path] = id;
        Serialize();
    }

    void ResourceRegistry::ResourceDeleted(ResourceId id)
    {
        auto& metadata = registry[id];

        resourcesByPath.erase(metadata.path);
        registry.erase(id);
        Serialize();
    }

    void ResourceRegistry::Serialize()
    {
        auto path = Project::GetResourceRegistryPath();

        YAML::Emitter out;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "ResourceRegistry" << YAML::Value;

            out << YAML::BeginSeq;
            for (const auto& [id, metadata] : registry)
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

    void ResourceRegistry::Deserialize()
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

            auto& metadata = registry[id];

            metadata.path = node["Path"].as<std::string>();
            metadata.type = StringToResourceType(node["Type"].as<std::string>());

            resourcesByPath[metadata.path] = id;
        }
    }

    std::unordered_map<ResourceId, ResourceMetadata> const& ResourceRegistry::GetResources()
    {
        return registry;
    }
};