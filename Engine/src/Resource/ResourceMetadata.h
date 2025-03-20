#pragma once

#include <Common/FileSystem.h>
#include <yaml-cpp/yaml.h>

template<>
struct YAML::convert<std::filesystem::path> {
    static Node encode(const std::filesystem::path& rhs) {
        Node node;
        node = rhs.string();
        return node;
    }

    static bool decode(const Node& node, std::filesystem::path& rhs) {
        rhs = node.as<std::string>();
        return true;
    }
};

namespace Engine
{
    class ResourceMetadata
    {
    public:
        void LoadFromFile(const std::filesystem::path& path)
        {
            root = YAML::LoadFile(path.string());
        }

        void SaveToFile(const std::filesystem::path& path) const
        {
            YAML::Emitter emitter;
            emitter << root;

            FileSystem::WriteFile(path, emitter.c_str());
        }

        template<typename T>
        void SetValue(const std::string& section, const std::string& key, const T& value)
        {
            root[section][key] = value;
        }

        template<typename T>
        T GetValue(const std::string& section, const std::string& key) const
        {
            return root[section][key].as<T>();
        }

        bool HasValue(const std::string& section, const std::string& key) const
        {
            return ! root[section][key].IsNull();
        }

    private:
        YAML::Node root;
        std::filesystem::path path;
    };
}
