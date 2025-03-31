#include "ResourceMetadata.h"

#include "Common/FileSystem.h"

namespace Engine
{
    void ResourceMetadata::LoadFromFile(const std::filesystem::path &path)
    {
        root = YAML::LoadFile(path.string());
    }

    void ResourceMetadata::SaveToFile(const std::filesystem::path &path) const
    {
        YAML::Emitter emitter;
        emitter << root;

        FileSystem::WriteFile(path, emitter.c_str());
    }

    bool ResourceMetadata::HasValue(const std::string &section, const std::string &key) const
    {
        return ! root[section][key].IsNull();
    }
}
