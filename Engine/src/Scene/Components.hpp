#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Rendering/Mesh.hpp"
#include "Rendering/Camera.hpp"

#include "Resource/Resource.hpp"

#include "Entity.hpp"

namespace Engine::Component
{
	struct Transform
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 GetLocalMatrix() const
		{
			return glm::translate(glm::mat4(1.0f), position)
				* glm::mat4_cast(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct Name
	{
		std::string name = "Empty Entity";
	};

	struct Relationship
	{
		std::size_t children{ 0 };
		entt::entity first{ entt::null };
		entt::entity prev{ entt::null };
		entt::entity next{ entt::null  };
		entt::entity parent{ entt::null };
	};

	struct MeshRender
	{
		ResourceId mesh{ 0 };
	};

	struct Camera
	{
		Engine::Camera camera;
	};

	struct DirectionalLight
	{
		glm::vec3 color{ 1.f };
		float intensity = 1.f;
	};

	struct PointLight
	{
		glm::vec3 color{ 1.f };
		float range = 1.f;
	};

	struct SkyLight
	{
		ResourceId cubemap{ 0 };
	};

	struct Script
	{
		ResourceId script{ 0 };
	};

	struct Delete {};

	template <typename... Component>
	struct Group { };

	using Serializable = Group<
		Transform,
		Name,
		Relationship,
		MeshRender,
		Camera,
		DirectionalLight,
		PointLight,
		SkyLight,
		Script
	>;

};

