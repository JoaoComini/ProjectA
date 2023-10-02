#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "Vertex.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"

#include "Material.hpp"

namespace Engine
{
	class Mesh
	{

	public:
		Mesh(const Vulkan::Device& device, const Material material, std::vector<Vertex> vertices, std::string path);

		void Draw(const VkCommandBuffer commandBuffer) const;

		void BuildIndexBuffer(void* indices, uint32_t size, uint32_t count, VkIndexType type);
		void BuildVertexBuffer(std::vector<Vertex> vertices);

		const Material& GetMaterial() const;

		const std::string& GetPath();
	private:

		size_t vertexCount;
		size_t indexCount;
		VkIndexType indexType;

		std::unique_ptr<Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<Vulkan::Buffer> indexBuffer;

		const Material material;
		const Vulkan::Device& device;
		const std::string path;
	};
}
