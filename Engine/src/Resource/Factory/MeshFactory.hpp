#pragma once

#include "Vulkan/Device.hpp"
#include "Rendering/Mesh.hpp"

#include <filesystem>

namespace Engine
{
	struct PrimitiveSpec
	{
		ResourceId material;
		std::vector<Vertex> vertices;
		std::vector<uint8_t> indices;
		VkIndexType indexType;
	};

	struct MeshSpec
	{
		std::vector<PrimitiveSpec> primitives;
	};

	class MeshFactory
	{
	public:
		MeshFactory(const Vulkan::Device& device);

		std::shared_ptr<Mesh> Create(std::filesystem::path destination, MeshSpec& spec);

		std::shared_ptr<Mesh> Load(std::filesystem::path source);

	private:

		std::shared_ptr<Mesh> BuildFromSpec(MeshSpec& spec);

		const Vulkan::Device& device;
	};
};