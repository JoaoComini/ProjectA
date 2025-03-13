#pragma once

#include "Common/Singleton.h"

#include "Resource.h"
#include <filesystem>


namespace Engine
{
	struct ResourceEntry
	{
		ResourceId id;
		ResourceMetadata metadata;
	};

	class ResourceRegistry : public Singleton<ResourceRegistry>
	{
	public:
		ResourceRegistry();

		bool HasResource(ResourceId id) const;
		bool HasResourceOnPath(const std::filesystem::path &path) const;

		ResourceMetadata* FindMetadataById(ResourceId id);
		ResourceId FindResourceByPath(const std::filesystem::path &path);
		std::vector<ResourceEntry> GetEntriesByType(ResourceType type);

		void ResourceCreated(ResourceId id, const ResourceMetadata &metadata);
		void ResourceDeleted(ResourceId id);

		void Serialize();
		void Deserialize();

		const std::unordered_map<ResourceId, ResourceMetadata>& GetResources() const;
	private:
		std::unordered_map<ResourceId, ResourceMetadata> registry;
		std::unordered_map<std::filesystem::path, ResourceId> resourcesByPath;
	};
}