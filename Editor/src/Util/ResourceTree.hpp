#pragma once

#include <Resource/Resource.hpp>

#include <map>

class ResourceTree
{
public:
	struct Node
	{
		Node(std::filesystem::path path, bool directory, Engine::ResourceId id = { 0 }, Engine::ResourceMetadata metadata = {})
			: path(path), directory(directory), id(id), metadata(metadata) {}

		std::filesystem::path path;
		bool directory;

		Engine::ResourceId id{ 0 };
		Engine::ResourceMetadata metadata;

		Node* parent = nullptr;
		std::map<std::filesystem::path, Node*> children;
	};

	void SetRoot(std::filesystem::path path);

	Node* Search(std::filesystem::path path);

	void DeleteNode(Node* node);

	void Refresh(std::filesystem::path path, Engine::ResourceId id, Engine::ResourceMetadata metadata);

private:
	void AddNode(Node* parent, Node* node);

	Node* root = nullptr;
};