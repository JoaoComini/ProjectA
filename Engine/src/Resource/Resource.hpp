#pragma once

#include "Common/Uuid.hpp"

namespace Engine
{
	using ResourceId = Uuid;

	enum class ResourceType : uint16_t
	{
		None = 0,
		Texture,
		Cubemap,
		Material,
		Mesh,
		Scene,
		Script,
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
		virtual ~Resource() {}
		ResourceId id{ 0 };
		virtual ResourceType GetType() const = 0;
	};
};
