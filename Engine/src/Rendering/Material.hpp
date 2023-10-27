#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"

#include "Resource/Resource.hpp"

namespace Engine
{
	class Material : public Resource
	{
	public:
		Material(ResourceId diffuse, ResourceId normal, glm::vec4 color = glm::vec4{1.f});
		~Material() = default;

		ResourceId GetDiffuse() const;

		ResourceId GetNormal() const;

		glm::vec4 GetColor() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Material;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

	private:
		ResourceId diffuse;
		ResourceId normal;
		glm::vec4 color{ 1.f };
	};
};