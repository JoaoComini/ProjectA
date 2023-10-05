#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <filesystem>

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"

#include "Resource/Resource.hpp"

#include "Material.hpp"
#include "Vertex.hpp"

namespace Engine
{
	class Mesh: public Resource
	{

	public:
		Mesh(const Vulkan::Device& device, ResourceId material, std::vector<Vertex> vertices, std::vector<uint8_t> indices, VkIndexType indexType =  VK_INDEX_TYPE_MAX_ENUM);

		void Draw(const VkCommandBuffer commandBuffer) const;

		ResourceId GetMaterial() const;

	private:

		void BuildIndexBuffer(std::vector<uint8_t> indices, VkIndexType type);
		void BuildVertexBuffer(std::vector<Vertex> vertices);

		size_t vertexCount;
		size_t indexCount;
		VkIndexType indexType;

		std::unique_ptr<Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<Vulkan::Buffer> indexBuffer;

		ResourceId material;

		const Vulkan::Device& device;
	};
}
