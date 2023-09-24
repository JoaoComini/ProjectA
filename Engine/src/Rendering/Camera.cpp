#include "Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Rendering
{
	Camera::Camera(float fov, float aspect, float near, float far) : fov(fov), aspect(aspect), near(near), far(far)
	{
		UpdateProjection();
	}

	void Camera::SetAspect(float aspect)
	{
		this->aspect = aspect;

		UpdateProjection();
	}

	glm::mat4 Camera::GetProjection() const
	{
		return this->projection;
	}

	void Camera::UpdateProjection()
	{
		this->projection = glm::perspective(fov, aspect, near, far);
		this->projection[1][1] *= -1;
	}
}