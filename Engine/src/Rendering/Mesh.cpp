#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"

#include "Utils/Hash.hpp"

#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

namespace Engine
{
    Mesh::Mesh(const Vulkan::Device& device, const Material material, std::vector<Vertex> vertices) : material(material), device(device)
    {
        BuildVertexBuffer(vertices);
    }

    void Mesh::BuildVertexBuffer(std::vector<Vertex> vertices)
    {
        vertexCount = vertices.size();

        vertexBuffer = Vulkan::BufferBuilder()
            .Size(sizeof(Vertex) * vertexCount)
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::VERTEX)
            .Build(device);

        vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertexCount);
    }

    void Mesh::BuildIndexBuffer(void* indices, uint32_t size, uint32_t count, VkIndexType type)
    {
        indexCount = count;
        indexType = type;

        indexBuffer = Vulkan::BufferBuilder()
            .Size(size)
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::INDEX)
            .Build(device);

        indexBuffer->SetData(indices, size);
    }

    void Mesh::Draw(const VkCommandBuffer commandBuffer) const
    {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer->GetHandle(), offsets);
         
        if (indexBuffer != nullptr)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetHandle(), 0, indexType);
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

            return;
        }

        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    const Material& Mesh::GetMaterial() const
    {
        return material;
    }

}
