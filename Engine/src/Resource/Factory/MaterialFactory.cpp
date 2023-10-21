#include "MaterialFactory.hpp"

#include "Common/FileSystem.hpp"

#include "../Flatbuffers/Material_generated.h"
#include "flatbuffers/flatbuffers.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Engine
{
    void MaterialFactory::Create(std::filesystem::path destination, MaterialSpec& spec)
    {
        std::span<const float, 4> color{ glm::value_ptr(spec.color), 4 };

        flatbuffers::MaterialT material;
        material.diffuse = spec.diffuse;
        material.color = std::make_unique<flatbuffers::Color>(color);

        flatbuffers::FlatBufferBuilder builder(128);
        auto offset = flatbuffers::Material::Pack(builder, &material);
        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });
    }

    std::shared_ptr<Material> MaterialFactory::Load(std::filesystem::path source)
    {
        auto file = FileSystem::ReadFile(source);

        auto material = flatbuffers::GetMaterial(file.c_str());

        glm::vec4 color{ 1.f };
        if (material->color())
        {
            color = glm::make_vec4(material->color()->value()->data());
        }

        return std::make_shared<Material>(material->diffuse(), color);
    }
};