#pragma once

#include <glm/glm.hpp>

#include <string_view>

#include "Scene/Node/Node.hpp"

#include "Resource.hpp"

namespace Engine
{

	class Prefab : public Resource
	{
	public:
		void SetNodes(std::vector<std::unique_ptr<Node>>&& nodes)
		{
			this->nodes = std::move(nodes);
		}

		Node* GetRoot()
		{
			return root;
		}

		void SetRoot(Node* node)
		{
			root = node;
		}

		static ResourceType GetStaticType()
		{
			return ResourceType::Prefab;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

		Node* Instantiate()
		{
			return root->Clone();
		}

	private:
		Node* root;
		std::vector<std::unique_ptr<Node>> nodes;
	};
}