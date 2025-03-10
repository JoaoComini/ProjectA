#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"

#include "Common/Hash.hpp"

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
            .BufferUsage(Vulkan::BufferUsageFlags::Vertex)
            .Build(device);

        vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertexCount);
    }

    void Primitive::AddIndexBuffer(std::vector<uint8_t> indices, VkIndexType type)
    {
        uint32_t size = indices.size();

        uint32_t typeSize{};
        switch (type)
        {
        case VK_INDEX_TYPE_UINT8_KHR:
            typeSize = sizeof(uint8_t);
            break;
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
            .BufferUsage(Vulkan::BufferUsageFlags::Index)
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
            commandBuffer.DrawIndexed(indexCount, 1, 0, 0, 0);

            return;
        }

        commandBuffer.Draw(vertexCount, 1, 0, 0);
    }

    void Primitive::SetMaterial(ResourceId material)
    {
        this->material = material;
    }

    ResourceId Primitive::GetMaterial() const
    {
        return material;
    }

    void Mesh::AddPrimitive(std::unique_ptr<Primitive> primitive)
    {
        primitives.push_back(std::move(primitive));
    }

    void Mesh::SetBounds(AABB bounds)
    {
        this->bounds = bounds;
    }

    AABB Mesh::GetBounds() const
    {
        return bounds;
    }

    std::vector<std::unique_ptr<Primitive>> const& Mesh::GetPrimitives() const
    {
        return primitives;
    }

    std::shared_ptr<Mesh> Mesh::BuiltIn::Cube(Vulkan::Device& device)
    {
        static std::vector<Vertex> vertices{
            {{ -1.0f,  1.0f, -1.0f }},
            {{ -1.0f, -1.0f, -1.0f }},
            {{ 1.0f, -1.0f, -1.0f  }},
            {{ 1.0f, -1.0f, -1.0f  }},
            {{ 1.0f,  1.0f, -1.0f  }},
            {{ -1.0f,  1.0f, -1.0f }},

            {{ -1.0f, -1.0f,  1.0f }},
            {{ -1.0f, -1.0f, -1.0f }},
            {{ -1.0f,  1.0f, -1.0f }},
            {{ -1.0f,  1.0f, -1.0f }},
            {{ -1.0f,  1.0f,  1.0f }},
            {{ -1.0f, -1.0f,  1.0f }},

            {{ 1.0f, -1.0f, -1.0f }},
            {{ 1.0f, -1.0f,  1.0f }},
            {{ 1.0f,  1.0f,  1.0f }},
            {{ 1.0f,  1.0f,  1.0f }},
            {{ 1.0f,  1.0f, -1.0f }},
            {{ 1.0f, -1.0f, -1.0f }},

            {{ -1.0f, -1.0f,  1.0f }},
            {{ -1.0f,  1.0f,  1.0f }},
            {{  1.0f,  1.0f,  1.0f }},
            {{  1.0f,  1.0f,  1.0f }},
            {{  1.0f, -1.0f,  1.0f }},
            {{ -1.0f, -1.0f,  1.0f }},

            {{ -1.0f,  1.0f, -1.0f }},
            {{  1.0f,  1.0f, -1.0f }},
            {{  1.0f,  1.0f,  1.0f }},
            {{  1.0f,  1.0f,  1.0f }},
            {{ -1.0f,  1.0f,  1.0f }},
            {{ -1.0f,  1.0f, -1.0f }},

            {{ -1.0f, -1.0f, -1.0f }},
            {{ -1.0f, -1.0f,  1.0f }},
            {{  1.0f, -1.0f, -1.0f }},
            {{  1.0f, -1.0f, -1.0f }},
            {{ -1.0f, -1.0f,  1.0f }},
            {{  1.0f, -1.0f,  1.0f }}
        };

        auto mesh = std::make_shared<Mesh>();

        auto primitive = std::make_unique<Primitive>(device);
        primitive->AddVertexBuffer(vertices);

        mesh->AddPrimitive(std::move(primitive));

        return mesh;
    }
}
