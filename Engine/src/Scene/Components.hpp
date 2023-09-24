#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Rendering/Mesh.hpp"
#include "Rendering/Camera.hpp"

namespace Scene::Component
{
	struct Transform
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 Get() const
		{
			return glm::translate(glm::mat4(1.0f), position)
				* glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z)
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};


	struct MeshRenderer
	{
		Rendering::Mesh* mesh;
	};

	struct OnWindowResize
	{
		int width;
		int height;
	};

	struct Camera
	{
		Rendering::Camera camera;
	};
};

