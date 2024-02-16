#pragma once

#include "Node/Node.hpp"

#include "Resource/Resource.hpp"

#include <vector>
#include <unordered_map>
#include <functional>

namespace Engine
{
	class Scene : public Resource
	{
	public:
		Scene();

		Node* GetRoot() const;

		void OnUpdate(float timestep);

		static ResourceType GetStaticType()
		{
			return ResourceType::Scene;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

	private:
		Node* root;
	};
};
