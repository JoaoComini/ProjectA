#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"

#include "Common/Hash.hpp"

#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

namespace Engine
{
    Mesh::Mesh(
        const Vulkan::Device& device,
        const ResourceId material,
        std::vector<Vertex> vertices,
        std::vector<uint8_t> indices,
        VkIndexType indexType
    ) : Resource(ResourceType::Mesh), material(material), device(device)
    {
        BuildVertexBuffer(vertices);
        BuildIndexBuffer(indices, indexType);
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

    void Mesh::BuildIndexBuffer(std::vector<uint8_t> indices, VkIndexType type)
    {
        uint32_t size = indices.size();

        uint32_t typeSize;
        switch (type)
        {
        case VK_INDEX_TYPE_UINT16:
            typeSize = sizeof(uint16_t);
            break;
        case VK_INDEX_TYPE_UINT32:
            typeSize = sizeof(uint32_t);
            break;
        default:
            break;
        }

        indexCount = indices.size() / typeSize;

        indexType = type;

        indexBuffer = Vulkan::BufferBuilder()
            .Size(size)
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::INDEX)
            .Build(device);

        indexBuffer->SetData(indices.data(), size);
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

    ResourceId Mesh::GetMaterial() const
    {
        return material;
    }
}
