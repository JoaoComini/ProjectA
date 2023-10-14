#include "MaterialFactory.hpp"

#include <fstream>

namespace Engine
{
    ResourceId MaterialFactory::Create(std::filesystem::path destination, MaterialSpec& spec)
    {
        ResourceId id;

        std::ofstream file(destination, std::ios::out | std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<char*>(&spec.diffuse), sizeof(spec.diffuse));
        file.close();

        return id;
    }
    std::shared_ptr<Material> MaterialFactory::Load(std::filesystem::path source)
    {
        MaterialSpec spec{};

        std::ifstream file(source, std::ios::in | std::ios::binary);
        file.read(reinterpret_cast<char*>(&spec.diffuse), sizeof(spec.diffuse));
        file.close();

        return std::make_shared<Material>(spec.diffuse);
    }
};