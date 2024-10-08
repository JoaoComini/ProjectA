#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include "Resource/Resource.hpp"

#include "Material.hpp"
#include "Vertex.hpp"

namespace Engine
{
	class Primitive
	{
	public:
		Primitive(const Vulkan::Device& device);

		void AddIndexBuffer(std::vector<uint8_t> indices, VkIndexType type);
		void AddVertexBuffer(std::vector<Vertex> vertices);

		void Draw(Vulkan::CommandBuffer& commandBuffer) const;

		void SetMaterial(ResourceId material);

		ResourceId GetMaterial() const;
	private:
		size_t vertexCount{ 0 };
		size_t indexCount{ 0 };
		VkIndexType indexType{ VK_INDEX_TYPE_MAX_ENUM };

		std::unique_ptr<Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<Vulkan::Buffer> indexBuffer;

		ResourceId material;

		const Vulkan::Device& device;
	};

	class Mesh: public Resource
	{
	public:
		void AddPrimitive(std::unique_ptr<Primitive> primitive);

		std::vector<std::unique_ptr<Primitive>> const& GetPrimitives() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Mesh;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

		static std::string GetExtension()
		{
			return "pares";
		}

		class BuiltIn
		{
		public:
			static std::shared_ptr<Mesh> Cube(Vulkan::Device& device);
		};

	private:
		std::vector<std::unique_ptr<Primitive>> primitives;
	};
}
