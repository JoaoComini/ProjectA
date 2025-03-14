#pragma once

#include "Resource.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

namespace Engine
{
    class ResourceSaver
    {
    public:
        template <typename T>
        static void Save(const std::filesystem::path& path, const T& resource)
        {
            static_assert(std::is_base_of_v<Resource, T>, "T must be derived from Resource");

            std::ofstream stream{ path, std::ios::binary };

            cereal::PortableBinaryOutputArchive archive{ stream };
            archive(resource);
        }
    };
}
