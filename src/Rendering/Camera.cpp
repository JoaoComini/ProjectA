#include "Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Rendering
{
	Camera::Camera(float fov, float aspect, float near, float far) : fov(fov), aspect(aspect), near(near), far(far)
	{
		UpdateProjection();

		this->view = glm::lookAt(position, glm::vec3(0), glm::vec3(0.f, 0.f, 0.1));
	}

	void Camera::SetPosition(glm::vec3 position)
	{
		this->position = position;
		this->view = glm::lookAt(position, glm::vec3(0), glm::vec3(0.f, 0.f, 0.1));
	}

	void Camera::SetAspect(float aspect)
	{
		this->aspect = aspect;

		UpdateProjection();
	}

	glm::mat4 Camera::GetViewProjection()
	{
		return this->projection * this->view;
	}

	void Camera::UpdateProjection()
	{
		this->projection = glm::perspective(fov, aspect, near, far);
		this->projection[1][1] *= -1;
	}
}