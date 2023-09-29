#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float near, float far);
		~Camera() = default;

		void SetAspectRatio(float aspectRatio);

		glm::mat4 GetProjection() const;

	private:
		void UpdateProjection();

		float fov;
		float aspectRatio;
		float near;
		float far;

		glm::mat4 projection;
	};
}