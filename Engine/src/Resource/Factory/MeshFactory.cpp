#include "MeshFactory.hpp"

#include "Common/FileSystem.hpp"

#include "../Flatbuffers/Mesh_generated.h"

#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
    MeshFactory::MeshFactory(const Vulkan::Device& device)
        : device(device)
    { }


    ResourceId MeshFactory::Create(std::filesystem::path destination, MeshSpec& spec)
    {
        ResourceId id;

        flatbuffers::MeshT mesh;

        for (auto& primitiveSpec: spec.primitives)
        {
            auto primitive = std::make_unique<flatbuffers::PrimitiveT>();

            primitive->material = primitiveSpec.material;
            primitive->vertices = primitiveSpec.vertices;
            primitive->indices = primitiveSpec.indices;
            primitive->index_type = primitiveSpec.indexType;

            mesh.primitives.push_back(std::move(primitive));
        }

        flatbuffers::FlatBufferBuilder builder(1024);

        auto offset = flatbuffers::Mesh::Pack(builder, &mesh);

        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });

        return id;
    }

    std::shared_ptr<Mesh> MeshFactory::Load(std::filesystem::path source)
    {
        auto file = FileSystem::ReadFile(source);

        flatbuffers::MeshT buffer;
        flatbuffers::GetMesh(file.c_str())->UnPackTo(&buffer);

        auto mesh = std::make_shared<Mesh>();

        for (auto& p: buffer.primitives)
        {
            auto primitive = std::make_unique<Primitive>(device);

            primitive->AddVertexBuffer(p->vertices);

            if (p->indices.size() > 0)
            {
                primitive->AddIndexBuffer(p->indices, static_cast<VkIndexType>(p->index_type));
            }

            if (p->material)
            {
                primitive->SetMaterial(p->material);
            }

            mesh->AddPrimitive(std::move(primitive));
        }

        return mesh;
    }

};