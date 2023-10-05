#include "Resource.hpp"

#include <type_traits>
#include <unordered_map>
namespace Engine
{
    std::string_view ResourceTypeToString(ResourceType type)
    {
        static std::unordered_map<ResourceType, std::string_view> map =
        {
            { ResourceType::Mesh, "Mesh" },
            { ResourceType::Texture, "Texture" },
            { ResourceType::Material, "Material" },
            { ResourceType::Model, "Model" },
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
            { "Model", ResourceType::Model },
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