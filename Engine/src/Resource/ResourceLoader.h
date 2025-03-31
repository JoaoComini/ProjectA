#pragma once

#include "Resource.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/adapters.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

namespace Engine
{
    class ResourceManager;

    class ResourceLoader
    {
    public:
        template <typename T>
        static std::shared_ptr<T> Load(const std::filesystem::path& path, ResourceManager& manager)
        {
            static_assert(std::is_base_of_v<Resource, T>, "T must be derived from Resource");

            std::ifstream stream{ path, std::ios::binary };

            auto resource = std::make_shared<T>();

            cereal::UserDataAdapter<ResourceManager, cereal::PortableBinaryInputArchive> archive{ manager, stream };
            archive(*resource);

            return resource;
        }
    };

}
