#include "Resource.hpp"

#include <type_traits>
#include <unordered_map>
namespace Engine
{
    std::string ResourceTypeToString(ResourceType type)
    {
        static std::unordered_map<ResourceType, std::string> map =
        {
            { ResourceType::Mesh, "Mesh" },
            { ResourceType::Texture, "Texture" },
            { ResourceType::Material, "Material" },
            { ResourceType::Prefab, "Prefab" },
            { ResourceType::None, "None" }
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
            { "Material", ResourceType::Material },
            { "Prefab", ResourceType::Prefab },
            { "None", ResourceType::None }
        };

        auto type = map.find(string);

        if (type != map.end())
        {
            return type->second;
        }

        return ResourceType::None;
    }
};