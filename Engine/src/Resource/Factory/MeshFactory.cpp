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
        file.write(reinterpret_cast<char*>(&spec.material), sizeof(spec.material));

        size_t verticesSize = spec.vertices.size() * sizeof(Vertex);
        file.write(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));
        file.write(reinterpret_cast<char*>(&spec.vertices[0]), verticesSize);

        size_t indicesSize = spec.indices.size();
        file.write(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
        file.write(reinterpret_cast<char*>(&spec.indices[0]), indicesSize);

        file.write(reinterpret_cast<char*>(&spec.indexType), sizeof(spec.indexType));
        file.close();

        return std::make_shared<Mesh>(device, spec.material, spec.vertices, spec.indices, spec.indexType);
    }

    std::shared_ptr<Mesh> MeshFactory::Load(std::filesystem::path source)
    {
        MeshSpec spec{};

        std::ifstream file(source, std::ios::in | std::ios::binary);
        file.read(reinterpret_cast<char*>(&spec.material), sizeof(spec.material));

        size_t verticesSize{};
        file.read(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));

        spec.vertices.resize(verticesSize);
        file.read(reinterpret_cast<char*>(&spec.vertices[0]), verticesSize);

        size_t indicesSize{};
        file.read(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));

        spec.indices.resize(indicesSize);
        file.read(reinterpret_cast<char*>(&spec.indices[0]), spec.indices.size());

        file.read(reinterpret_cast<char*>(&spec.indexType), sizeof(spec.indexType));
        file.close();

        return std::make_shared<Mesh>(device, spec.material, spec.vertices, spec.indices, spec.indexType);
    }

};