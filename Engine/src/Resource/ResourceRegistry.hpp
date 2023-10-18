#pragma once

#include "Common/Singleton.hpp"

#include "Resource.hpp"

#include <unordered_map>
#include <filesystem>

namespace Engine
{

	class ResourceRegistry : public Singleton<ResourceRegistry>
	{
	public:
		bool HasResource(ResourceId id);
		bool HasResourceOnPath(std::filesystem::path path);

		ResourceMetadata* FindMetadataById(ResourceId id);
		ResourceId FindResourceByPath(std::filesystem::path path);

		void ResourceCreated(ResourceId id, ResourceMetadata metadata);
		void ResourceDeleted(ResourceId id);

		void Serialize();
		void Deserialize();

		std::unordered_map<ResourceId, ResourceMetadata> const& GetResources();
	private:
		std::unordered_map<ResourceId, ResourceMetadata> registry;
		std::unordered_map<std::filesystem::path, ResourceId> resourcesByPath;
	};
};