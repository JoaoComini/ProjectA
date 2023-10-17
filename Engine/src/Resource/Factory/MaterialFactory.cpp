#include "MaterialFactory.hpp"

#include "Common/FileSystem.hpp"

#include "../Flatbuffers/Material_generated.h"
#include "flatbuffers/flatbuffers.h"

#include <fstream>

namespace Engine
{
    ResourceId MaterialFactory::Create(std::filesystem::path destination, MaterialSpec& spec)
    {
        ResourceId id;

        flatbuffers::MaterialT material;
        material.diffuse = spec.diffuse;

        flatbuffers::FlatBufferBuilder builder(128);
        auto offset = flatbuffers::Material::Pack(builder, &material);
        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });

        return id;
    }
    std::shared_ptr<Material> MaterialFactory::Load(std::filesystem::path source)
    {
        auto file = FileSystem::ReadFile(source);

        auto material = flatbuffers::GetMaterial(file.c_str());

        return std::make_shared<Material>(material->diffuse());
    }
};