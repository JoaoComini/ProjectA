#include "MeshFactory.h"

#include "Common/FileSystem.h"

#include "../Flatbuffers/Mesh_generated.h"

namespace Engine
{
    MeshFactory::MeshFactory(const Vulkan::Device& device)
        : device(device)
    { }


    void MeshFactory::Create(std::filesystem::path destination, MeshSpec& spec)
    {
        flatbuffers::MeshT mesh;

        glm::vec3 min{ std::numeric_limits<float>::max() }, max{ std::numeric_limits<float>::min() };

        for (auto& primitiveSpec: spec.primitives)
        {
            auto primitive = std::make_unique<flatbuffers::PrimitiveT>();

            primitive->material = primitiveSpec.material;
            primitive->vertices = primitiveSpec.vertices;
            primitive->indices = primitiveSpec.indices;
            primitive->index_type = primitiveSpec.indexType;

            for (auto& vertex : primitive->vertices)
            {
                min = glm::min(vertex.position, min);
                max = glm::max(vertex.position, max);
            }

            mesh.primitives.push_back(std::move(primitive));
        }

        mesh.bounds = std::make_unique<flatbuffers::AABB>(std::span<float, 3>(min), std::span<float, 3>(max));

        flatbuffers::FlatBufferBuilder builder(1024);

        auto offset = flatbuffers::Mesh::Pack(builder, &mesh);

        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });
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

            primitive->SetVertexBuffer(p->vertices);

            if (p->indices.size() > 0)
            {
                primitive->SetIndexBuffer(p->indices, static_cast<VkIndexType>(p->index_type));
            }

            if (p->material)
            {
                primitive->SetMaterial(p->material);
            }

            mesh->AddPrimitive(std::move(primitive));
        }

        mesh->SetBounds(AABB{ glm::make_vec3(buffer.bounds->min()->data()), glm::make_vec3(buffer.bounds->max()->data())});

        return mesh;
    }

};