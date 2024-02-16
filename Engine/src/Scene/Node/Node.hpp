#pragma once

#include "Common/Object.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <string_view>
#include <set>

namespace Engine
{
	class Scene;

	class Node : public Object
	{

	REFLECT(Node)

	public:
		Node();
		Node(const Node&);

		void SetName(const std::string& name);
		std::string GetName() const;

		void AddChild(Node* node);
		void RemoveChild(Node* node);

		const std::set<Node*> GetChildren() const;

		void SetScene(Scene* scene);

		Node* GetParent();

		virtual Node* Clone() const;

	protected:
		virtual void OnSceneAdd() {}
		virtual void OnSceneRemove() {}

	private:
		std::string name{ "Node" };

		Node* parent{ nullptr };

		Scene* scene{ nullptr };

		std::set<Node*> children;
	};
}