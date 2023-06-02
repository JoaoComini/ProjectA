#include "Mesh.hpp"

#include "Utils/Hash.hpp"

#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

#include <tiny_obj_loader.h>

namespace Rendering
{

    Mesh::Mesh(Vulkan::Device& device, std::string path)
    {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), nullptr);

        if (!warn.empty())
        {
            std::cout << "WARN: " << warn << std::endl;
        }

        if (!err.empty())
        {
            std::stringstream ss;
            ss << "failed to load " << path << ": " << err;
            throw std::runtime_error(ss.str());
        }

        std::unordered_map<Vertex, uint32_t> uniques{};
        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                tinyobj::real_t vx = attrib.vertices[3L * index.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3L * index.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3L * index.vertex_index + 2];

                tinyobj::real_t nx = attrib.normals[3L * index.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3L * index.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3L * index.normal_index + 2];

                tinyobj::real_t tx = attrib.texcoords[2L * index.texcoord_index + 0];
                tinyobj::real_t ty = 1.f - attrib.texcoords[2L * index.texcoord_index + 1];

                Vertex vertex{
                    .position = {vx, vy, vz},
                    .normal = {nx, ny, nz},
                    .uv = {tx, ty},
                };

                if (uniques.count(vertex) == 0)
                {
                    uniques[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniques[vertex]);
            }
        }

        vertexBuffer = Vulkan::BufferBuilder()
            .Size(sizeof(Vertex) * vertices.size())
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::VERTEX)
            .Build(device);

        vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

        indexBuffer = Vulkan::BufferBuilder()
            .Size(sizeof(int32_t) * indices.size())
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::INDEX)
            .Build(device);

        indexBuffer->SetData(indices.data(), sizeof(int32_t) * indices.size());
    }

    void Mesh::Draw(const VkCommandBuffer commandBuffer)
    {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer->GetHandle(), offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
    }
}
