#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include "Resource/Resource.hpp"

#include "Material.hpp"
#include "Vertex.hpp"

namespace Engine
{


	class AABB
	{

	public:
		AABB() {}
		AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max)
		{
			scale = max - min;
		}

		glm::vec3 GetCenter() const
		{
			return (min + max) * 0.5f;
		}

		void Transform(const glm::mat4& matrix)
		{
			glm::vec3 min = this->min;
			glm::vec3 max = this->max;

			this->min = glm::vec3{ std::numeric_limits<float>::max() };
			this->max = glm::vec3{ std::numeric_limits<float>::min() };

			Update(matrix * glm::vec4(min, 1.0f));
			Update(matrix * glm::vec4(min.x, min.y, max.z, 1.0f));
			Update(matrix * glm::vec4(min.x, max.y, min.z, 1.0f));
			Update(matrix * glm::vec4(min.x, max.y, max.z, 1.0f));
			Update(matrix * glm::vec4(max.x, min.y, min.z, 1.0f));
			Update(matrix * glm::vec4(max.x, min.y, max.z, 1.0f));
			Update(matrix * glm::vec4(max.x, max.y, min.z, 1.0f));
			Update(matrix * glm::vec4(max, 1.0f));

			scale = this->max - this->min;
		}

	private:
		void Update(const glm::vec3& point)
		{
			min = glm::min(min, point);
			max = glm::max(max, point);
		}

		glm::vec3 min{ 0 };
		glm::vec3 max{ 0 };

		glm::vec3 scale{ 0 };
	};

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
		void SetBounds(AABB bounds);
		AABB GetBounds() const;

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
		AABB bounds;
	};
}
