#include "Resource.h"

namespace Engine
{
    std::string ResourceTypeToString(ResourceType type)
    {
        static std::unordered_map<ResourceType, std::string> map =
        {
            { ResourceType::Mesh, "Mesh" },
            { ResourceType::Texture, "Texture" },
            { ResourceType::Cubemap, "Cubemap" },
            { ResourceType::Material, "Material" },
            { ResourceType::Scene, "Scene" },
            { ResourceType::None, "None" },
            { ResourceType::Script, "Script" }
        };

        auto string = map.find(type);

        if (string != map.end())
        {
            return string->second;
        }

        return "None";
    }

    ResourceType StringToResourceType(std::string_view string)
    {
        static std::unordered_map<std::string_view, ResourceType> map =
        {
            { "Mesh", ResourceType::Mesh },
            { "Texture", ResourceType::Texture },
            { "Cubemap", ResourceType::Cubemap},
            { "Material", ResourceType::Material },
            { "Scene", ResourceType::Scene },
            { "None", ResourceType::None },
            { "Script", ResourceType::Script }
        };

        auto type = map.find(string);

        if (type != map.end())
        {
            return type->second;
        }

        return ResourceType::None;
    }

    void Resource::SetId(ResourceId id)
    {
        this->id = id;
    }

    ResourceId Resource::GetId() const
    {
        return id;
    }
};