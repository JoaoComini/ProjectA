#pragma once

#include "Vulkan/Device.hpp"
#include "Rendering/Mesh.hpp"

#include "Factory.hpp"

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

	class MeshFactory : Factory<Mesh, MeshSpec>
	{
	public:
		MeshFactory(const Vulkan::Device& device);

		void Create(std::filesystem::path destination, MeshSpec& spec) override;

		std::shared_ptr<Mesh> Load(std::filesystem::path source) override;

	private:
		std::shared_ptr<Mesh> BuildFromSpec(MeshSpec& spec);

		const Vulkan::Device& device;
	};
};