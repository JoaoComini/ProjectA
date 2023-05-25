#pragma once

#include <glm/glm.hpp>

namespace Rendering
{
	class Camera
	{
	public:
		Camera(float fov, float aspect, float near, float far);
		~Camera() = default;

		void SetPosition(glm::vec3 position);

		glm::mat4 GetViewProjection();

	private:
		glm::mat4 projection;
		glm::mat4 view{ 1 };
		glm::vec3 position{ 2 };

	};
}