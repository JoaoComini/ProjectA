#pragma once

#include "Common/Uuid.hpp"

#include <memory>
#include <filesystem>
#include <string_view>

namespace Engine
{
	using ResourceId = Uuid;

	enum class ResourceType : uint16_t
	{
		None = 0,
		Texture,
		Material,
		Mesh,
		Model,
	};

	std::string ResourceTypeToString(ResourceType type);
	ResourceType StringToResourceType(std::string_view type);

	struct ResourceMetadata
	{
		std::filesystem::path path;
		ResourceType type;
	};

	class Resource
	{
	public:
		Resource(ResourceType type) : type(type) {}

		ResourceType GetType()
		{
			return type;
		}

	private:
		ResourceType type;
	};
};
