#pragma once

#include <glm/glm.hpp>

namespace Rendering
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspect, float near, float far);
		~Camera() = default;

		void SetAspect(float aspect);

		glm::mat4 GetProjection() const;

	private:
		void UpdateProjection();

		float fov;
		float aspect;
		float near;
		float far;

		glm::mat4 projection;
	};
}