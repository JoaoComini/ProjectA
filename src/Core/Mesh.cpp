#include "Mesh.hpp"

#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

#include <tiny_obj_loader.h>

Mesh::Mesh(Vulkan::Device &device, std::string path)
{

    tinyobj::attrib_t attrib;
    // shapes contains the info for each separate object in the file
    std::vector<tinyobj::shape_t> shapes;
    // materials contains the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    // error and warning output from the load function
    std::string warn;
    std::string err;

    // load the OBJ file
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), nullptr);
    // make sure to output the warnings to the console, in case there are issues with the file
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }
    // if we have any error, print it to the console, and break the mesh loading.
    // This happens if the file can't be found or is malformed
    if (!err.empty())
    {
        std::stringstream ss;
        ss << "failed to load " << path << ": " << err;
        throw std::runtime_error(ss.str());
    }

    std::unordered_map<Vertex, uint32_t> uniques{};
    for (const auto& shape : shapes)
    {
        // Loop over faces(polygon)
        for (const auto& index : shape.mesh.indices)
        {
            // vertex position
            tinyobj::real_t vx = attrib.vertices[3L * index.vertex_index + 0];
            tinyobj::real_t vy = attrib.vertices[3L * index.vertex_index + 1];
            tinyobj::real_t vz = attrib.vertices[3L * index.vertex_index + 2];
            // vertex normal
            tinyobj::real_t nx = attrib.normals[3L * index.normal_index + 0];
            tinyobj::real_t ny = attrib.normals[3L * index.normal_index + 1];
            tinyobj::real_t nz = attrib.normals[3L * index.normal_index + 2];

            // copy it into our vertex
            Vertex vertex{
                .position = {vx, vy, vz},
                .normal = {nx, ny, nz},
                .color = {nx, ny, nz},
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
        .Data(vertices.data())
        .Size(sizeof(Vertex) * vertices.size())
        .AllocationCreate(Vulkan::AllocationCreateFlags::MAPPED_BAR)
        .BufferUsage(Vulkan::BufferUsageFlags::VERTEX)
        .Build(device);

    indexBuffer = Vulkan::BufferBuilder()
        .Data(indices.data())
        .Size(sizeof(int32_t) * indices.size())
        .AllocationCreate(Vulkan::AllocationCreateFlags::MAPPED_BAR)
        .BufferUsage(Vulkan::BufferUsageFlags::INDEX)
        .Build(device);
}

void Mesh::Draw(const VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = { vertexBuffer->GetHandle() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
}
