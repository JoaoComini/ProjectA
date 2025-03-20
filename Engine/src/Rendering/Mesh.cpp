#include "Mesh.h"

#include <utility>

#include "Common/Hash.h"

namespace Engine
{
    void Primitive::SetVertices(std::vector<Vertex>&& vertices)
    {
        vertexCount = vertices.size();

        this->vertices = std::move(vertices);
    }

    void Primitive::SetIndices(std::vector<uint8_t>&& indices, const VkIndexType type)
    {
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

        this->indices = std::move(indices);
    }

    void Primitive::UploadToGpu(Vulkan::Device &device)
    {
        vertexBuffer = Vulkan::BufferBuilder()
            .Size(sizeof(Vertex) * vertexCount)
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::Vertex)
            .Build(device);

        vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertexCount);

        vertices.clear();
        vertices.shrink_to_fit();

        indexBuffer = Vulkan::BufferBuilder()
            .Size(indices.size())
            .Persistent()
            .AllowTransfer()
            .SequentialWrite()
            .BufferUsage(Vulkan::BufferUsageFlags::Index)
            .Build(device);

        indexBuffer->SetData(indices.data(), indices.size());

        indices.clear();
        indices.shrink_to_fit();
    }

    void Primitive::Draw(Vulkan::CommandBuffer& commandBuffer) const
    {
        constexpr VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer.GetHandle(), 0, 1, &vertexBuffer->GetHandle(), offsets);
         
        if (indexBuffer != nullptr)
        {
            vkCmdBindIndexBuffer(commandBuffer.GetHandle(), indexBuffer->GetHandle(), 0, indexType);
            commandBuffer.DrawIndexed(indexCount, 1, 0, 0, 0);

            return;
        }

        commandBuffer.Draw(vertexCount, 1, 0, 0);
    }

    void Primitive::SetMaterial(std::shared_ptr<Material> material)
    {
        this->material = std::move(material);
    }

    Material* Primitive::GetMaterial() const
    {
        return material.get();
    }

    void Mesh::UploadToGpu(Vulkan::Device &device)
    {
        for (auto& primitive : primitives)
        {
            primitive.UploadToGpu(device);
        }
    }

    void Mesh::AddPrimitive(Primitive&& primitive)
    {
        primitives.push_back(std::move(primitive));
    }

    void Mesh::SetBounds(const AABB &bounds)
    {
        this->bounds = bounds;
    }

    AABB Mesh::GetBounds() const
    {
        return bounds;
    }

    const std::vector<Primitive>& Mesh::GetPrimitives() const
    {
        return primitives;
    }

    std::shared_ptr<Mesh> Mesh::BuiltIn::Cube(Vulkan::Device& device)
    {
        std::vector<Vertex> vertices {
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

        Primitive primitive{};
        primitive.SetVertices(std::move(vertices));

        mesh->AddPrimitive(std::move(primitive));

        mesh->UploadToGpu(device);

        return mesh;
    }
}
