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
    Primitive::Primitive(const Vulkan::Device& device)
        : device(device)
    {
    }

    void Primitive::AddVertexBuffer(std::vector<Vertex> vertices)
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

    void Primitive::AddIndexBuffer(std::vector<uint8_t> indices, VkIndexType type)
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

    void Primitive::Draw(Vulkan::CommandBuffer& commandBuffer) const
    {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer.GetHandle(), 0, 1, &vertexBuffer->GetHandle(), offsets);
         
        if (indexBuffer != nullptr)
        {
            vkCmdBindIndexBuffer(commandBuffer.GetHandle(), indexBuffer->GetHandle(), 0, indexType);
            vkCmdDrawIndexed(commandBuffer.GetHandle(), indexCount, 1, 0, 0, 0);

            return;
        }

        vkCmdDraw(commandBuffer.GetHandle(), vertexCount, 1, 0, 0);
    }

    void Primitive::SetMaterial(ResourceId material)
    {
        this->material = material;
    }

    ResourceId Primitive::GetMaterial() const
    {
        return material;
    }

    Mesh::Mesh() : Resource(ResourceType::Mesh)
    {
    }

    void Mesh::AddPrimitive(std::unique_ptr<Primitive> primitive)
    {
        primitives.push_back(std::move(primitive));
    }

    std::vector<std::unique_ptr<Primitive>> const& Mesh::GetPrimitives() const
    {
        return primitives;
    }
}
