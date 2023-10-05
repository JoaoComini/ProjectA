#pragma once

#include <glm/glm.hpp>

#include <string_view>

#include "Scene/Components.hpp"

#include "Resource.hpp"

namespace Engine
{
	struct Node
	{

	public:
		void SetName(std::string name)
		{
			this->name.name = name;
		}

		std::string_view GetName()
		{
			return name.name;
		}

		void SetMesh(ResourceId id)
		{
			this->meshRender.mesh = id;
		}

		ResourceId GetMesh()
		{
			return meshRender.mesh;
		}

		Component::Transform& GetTransform()
		{
			return transform;
		}

		void SetParent(Node& parent)
		{
			this->parent = &parent;
		}

		void AddChild(Node& child)
		{
			children.push_back(&child);
		}

		const std::vector<Node*> GetChildren() const
		{
			return children;
		}

	private:
		Component::Name name;
		Component::Transform transform;
		Component::MeshRender meshRender;

		Node* parent;
		std::vector<Node *> children;
	};

	class Model : public Resource
	{
	public:
		Model();

		void SetNodes(std::vector<std::unique_ptr<Node>>&& nodes)
		{
			this->nodes = std::move(nodes);
		}

		Node& GetRoot()
		{
			return *root;
		}

		void SetRoot(Node& node)
		{
			root = &node;
		}

	private:
		Node* root;
		std::vector<std::unique_ptr<Node>> nodes;
	};
}