#include "MeshFactory.hpp"

#include <fstream>

namespace Engine
{
    MeshFactory::MeshFactory(const Vulkan::Device& device)
        : device(device)
    { }


    std::shared_ptr<Mesh> MeshFactory::Create(std::filesystem::path destination, MeshSpec& spec)
    {
        std::ofstream file(destination, std::ios::out | std::ios::binary | std::ios::trunc);

        size_t size = spec.primitives.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(size));

        for (auto& primitive: spec.primitives)
        {
            file.write(reinterpret_cast<char*>(&primitive.material), sizeof(primitive.material));

            size_t verticesSize = primitive.vertices.size() * sizeof(Vertex);
            file.write(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));
            file.write(reinterpret_cast<char*>(&primitive.vertices[0]), verticesSize);

            size_t indicesSize = primitive.indices.size();
            file.write(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
            file.write(reinterpret_cast<char*>(&primitive.indices[0]), indicesSize);

            file.write(reinterpret_cast<char*>(&primitive.indexType), sizeof(primitive.indexType));
        }
        file.close();

        return BuildFromSpec(spec);
    }

    std::shared_ptr<Mesh> MeshFactory::Load(std::filesystem::path source)
    {
        MeshSpec spec{};

        std::ifstream file(source, std::ios::in | std::ios::binary);

        size_t size{ 0 };
        file.read(reinterpret_cast<char*>(&size), sizeof(size));

        for (size_t i = 0; i < size; i++)
        {
            PrimitiveSpec primitiveSpec{};

            file.read(reinterpret_cast<char*>(&primitiveSpec.material), sizeof(primitiveSpec.material));

            size_t verticesSize{};
            file.read(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));

            primitiveSpec.vertices.resize(verticesSize);
            file.read(reinterpret_cast<char*>(&primitiveSpec.vertices[0]), verticesSize);

            size_t indicesSize{};
            file.read(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));

            primitiveSpec.indices.resize(indicesSize);
            file.read(reinterpret_cast<char*>(&primitiveSpec.indices[0]), primitiveSpec.indices.size());

            file.read(reinterpret_cast<char*>(&primitiveSpec.indexType), sizeof(primitiveSpec.indexType));
            file.close();
        }

        return BuildFromSpec(spec);
    }

    std::shared_ptr<Mesh> MeshFactory::BuildFromSpec(MeshSpec& spec)
    {
        auto mesh = std::make_shared<Mesh>();

        for (auto& primitiveSpec : spec.primitives)
        {
            auto primitive = std::make_unique<Primitive>(device);

            primitive->AddVertexBuffer(primitiveSpec.vertices);

            if (primitiveSpec.indices.size() > 0)
            {
                primitive->AddIndexBuffer(primitiveSpec.indices, primitiveSpec.indexType);
            }

            if (primitiveSpec.material)
            {
                primitive->SetMaterial(primitiveSpec.material);
            }

            mesh->AddPrimitive(std::move(primitive));
        }

        return mesh;
    }

};