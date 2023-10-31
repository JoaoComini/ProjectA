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

	struct Id
	{
		Uuid id;
	};

	struct Relationship
	{
		std::size_t children;
		Entity first;
		Entity prev;
		Entity next;
		Entity parent;
	};

	struct Delete{};

	struct MeshRender
	{
		ResourceId mesh{ 0 };
	};

	struct Camera
	{
		Engine::Camera* camera;
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

};

