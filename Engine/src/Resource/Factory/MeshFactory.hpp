#pragma once

#include "Vulkan/Device.hpp"
#include "Rendering/Mesh.hpp"

#include <filesystem>

namespace Engine
{

	struct MeshSpec
	{
		ResourceId material;
		std::vector<Vertex> vertices;
		std::vector<uint8_t> indices;
		VkIndexType indexType;
	};

	class MeshFactory
	{
	public:
		MeshFactory(const Vulkan::Device& device);

		std::shared_ptr<Mesh> Create(std::filesystem::path destination, MeshSpec& spec);

		std::shared_ptr<Mesh> Load(std::filesystem::path source);

	private:
		const Vulkan::Device& device;
	};
};