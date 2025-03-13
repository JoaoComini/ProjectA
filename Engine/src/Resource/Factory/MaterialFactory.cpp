#include "MaterialFactory.h"

#include "Common/FileSystem.h"

#include "../Flatbuffers/Material_generated.h"
#include "flatbuffers/flatbuffers.h"

namespace Engine
{
    void MaterialFactory::Create(std::filesystem::path destination, MaterialSpec& spec)
    {
        std::span<const float, 4> color{ glm::value_ptr(spec.albedoColor), 4 };

        flatbuffers::MaterialT material;
        material.albedo_texture = spec.albedoTexture;
        material.normal_texture = spec.normalTexture;
        material.metallic_roughness_texture = spec.metallicRoughnessTexture;
        material.albedo_color = std::make_unique<flatbuffers::Color>(color);
        material.metallic_factor = spec.metallicFactor;
        material.roughness_factor = spec.roughnessFactor;
        material.alpha_mode = static_cast<flatbuffers::AlphaMode>(spec.alphaMode);
        material.alpha_cutoff = spec.alphaCutoff;

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

        auto color = glm::make_vec4(material->albedo_color()->value()->data());
        
        return std::make_shared<Material>(
            material->albedo_texture(),
            material->normal_texture(),
            material->metallic_roughness_texture(),
            color,
            material->metallic_factor(),
            material->roughness_factor(),
            static_cast<AlphaMode>(material->alpha_mode()),
            material->alpha_cutoff()
        );
    }
};