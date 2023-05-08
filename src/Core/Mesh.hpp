#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    bool operator==(const Vertex& other) const
    {
        return position == other.position && color == other.color && normal == other.normal;
    }
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec3>()(vertex.normal) << 1);
        }
    };
}

class Mesh
{

public:
    Mesh(Vulkan::Device &device, std::string path);

    void Draw(const VkCommandBuffer commandBuffer);
private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unique_ptr<Vulkan::Buffer> vertexBuffer;
    std::unique_ptr<Vulkan::Buffer> indexBuffer;

};
