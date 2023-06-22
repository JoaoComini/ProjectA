#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "Vertex.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"

namespace Rendering
{
	class Mesh
	{

	public:
		Mesh(Vulkan::Device& device, std::string path);

		void Draw(const VkCommandBuffer commandBuffer);
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::unique_ptr<Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<Vulkan::Buffer> indexBuffer;
	};
}
