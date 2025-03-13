#include "ResourceRegistry.h"

#include <Common/FileSystem.h>

#include "Project/Project.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
    ResourceRegistry::ResourceRegistry()
    {
    }

    bool ResourceRegistry::HasResource(const ResourceId id) const
    {
        return registry.contains(id);
    }

    bool ResourceRegistry::HasResourceOnPath(const std::filesystem::path &path) const
    {
        return resourcesByPath.contains(path);
    }

    ResourceMetadata* ResourceRegistry::FindMetadataById(const ResourceId id)
    {
        if (HasResource(id))
        {
            return &registry[id];
        }

        return nullptr;
    }

    ResourceId ResourceRegistry::FindResourceByPath(const std::filesystem::path &path)
    {
        if (HasResourceOnPath(path))
        {
            return resourcesByPath[path];
        }

        return { 0 };
    }

    std::vector<ResourceEntry> ResourceRegistry::GetEntriesByType(const ResourceType type)
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

    void ResourceRegistry::ResourceCreated(const ResourceId id, const ResourceMetadata &metadata)
    {
        registry[id] = metadata;
        resourcesByPath[metadata.path] = id;
        Serialize();
    }

    void ResourceRegistry::ResourceDeleted(const ResourceId id)
    {
        auto& metadata = registry[id];

        resourcesByPath.erase(metadata.path);
        registry.erase(id);
        Serialize();
    }

    void ResourceRegistry::Serialize()
    {
        const auto path = Project::GetResourceRegistryPath();

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

        std::ofstream file(path);
        file << out.c_str();
    }

    void ResourceRegistry::Deserialize()
    {
        const auto path = Project::GetResourceRegistryPath();
        YAML::Node file;
        try
        {
            file = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            return;
        }

        auto root = file["ResourceRegistry"];

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

    const std::unordered_map<ResourceId, ResourceMetadata>& ResourceRegistry::GetResources() const
    {
        return registry;
    }
};