#pragma once

namespace Engine
{

    class ResourceImporter
    {
    public:
        virtual ~ResourceImporter() = default;
        virtual void Import(const std::filesystem::path& source, const std::filesystem::path& destination) = 0;
        virtual std::vector<std::string> GetSupportedExtensions() const = 0;
    };

}
