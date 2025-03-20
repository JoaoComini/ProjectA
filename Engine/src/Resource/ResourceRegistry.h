#pragma once

#include "Common/Singleton.h"

#include "Resource.h"
#include <filesystem>


namespace Engine
{
	struct ResourceEntry
	{
		ResourceId id;
		ResourceMapping metadata;
	};

	class ResourceRegistry : public Singleton<ResourceRegistry>
	{
	public:
		bool HasResource(ResourceId id) const;
		bool HasResourceOnPath(const std::filesystem::path &path) const;

		const ResourceMapping* FindMappingById(ResourceId id);
		ResourceId FindResourceByPath(const std::filesystem::path &path);
		std::vector<ResourceEntry> GetEntriesByType(ResourceType type);

		void ResourceCreated(ResourceId id, const ResourceMapping &metadata);
		void ResourceDeleted(ResourceId id);

		void Serialize();
		void Deserialize();

		const std::unordered_map<ResourceId, ResourceMapping>& GetResources() const;
	private:
		std::unordered_map<ResourceId, ResourceMapping> registry;
		std::unordered_map<std::filesystem::path, ResourceId> resourcesByPath;
	};
}