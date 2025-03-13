#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/CommandBuffer.h"

#include "Resource/Resource.h"

#include "Material.h"
#include "Vertex.h"

namespace Engine
{
	class AABB
	{

	public:
		AABB() = default;
		AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max)
		{
			scale = max - min;
		}

		[[nodiscard]] glm::vec3 GetCenter() const
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
		void SetIndices(std::vector<uint8_t>&& indices, VkIndexType type);
		void SetVertices(std::vector<Vertex>&& vertices);

		void UploadToGpu(Vulkan::Device& device);

		void Draw(Vulkan::CommandBuffer& commandBuffer) const;

		void SetMaterial(std::shared_ptr<Material> material);

		[[nodiscard]] Material* GetMaterial() const;
	private:
		std::vector<uint8_t> indices;
		std::vector<Vertex> vertices;

		size_t vertexCount{ 0 };
		size_t indexCount{ 0 };
		VkIndexType indexType{ VK_INDEX_TYPE_MAX_ENUM };

		std::unique_ptr<Vulkan::Buffer> vertexBuffer;
		std::unique_ptr<Vulkan::Buffer> indexBuffer;

		std::shared_ptr<Material> material;
	};

	class Mesh final : public Resource
	{
	public:
		void UploadToGpu(Vulkan::Device& device);

		void AddPrimitive(std::unique_ptr<Primitive> primitive);
		void SetBounds(const AABB &bounds);
		[[nodiscard]] AABB GetBounds() const;

		[[nodiscard]] std::vector<std::unique_ptr<Primitive>> const& GetPrimitives() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Mesh;
		}

		[[nodiscard]] ResourceType GetType() const override
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
