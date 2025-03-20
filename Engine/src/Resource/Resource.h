#pragma once

#include "Common/Uuid.h"

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

	struct ResourceMapping
	{
		std::filesystem::path path;
		ResourceType type;
	};

	class Resource
	{
	public:
		Resource() = default;
		virtual ~Resource() = default;
		[[nodiscard]] virtual ResourceType GetType() const = 0;

		void SetId(ResourceId id);
		[[nodiscard]] ResourceId GetId() const;
	private:
		ResourceId id{ 0 };
	};
};
