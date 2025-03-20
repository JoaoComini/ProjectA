#pragma once

#include <Resource/Resource.h>

#include <map>

class ResourceTree
{
public:
	struct Node
	{
		Node(std::filesystem::path path, bool directory, const Engine::ResourceId id = Engine::ResourceId{ 0 }, const Engine::ResourceMapping &mapping = {})
			: path(path), directory(directory), id(id), mapping(mapping) {}

		std::filesystem::path path;
		bool directory;

		Engine::ResourceId id{ 0 };
		Engine::ResourceMapping mapping;

		Node* parent = nullptr;
		std::map<std::filesystem::path, Node*> children;
	};

	void SetRoot(std::filesystem::path path);

	Node* Search(std::filesystem::path path);

	void DeleteNode(Node* node);

	void Refresh(std::filesystem::path path, Engine::ResourceId id, Engine::ResourceMapping mapping);

private:
	void AddNode(Node* parent, Node* node);

	Node* root = nullptr;
};