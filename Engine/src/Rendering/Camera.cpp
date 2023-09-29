#include "Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Engine
{
	Camera::Camera(float fov, float aspectRatio, float near, float far) : fov(fov), aspectRatio(aspectRatio), near(near), far(far)
	{
		UpdateProjection();
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		this->aspectRatio = aspectRatio;

		UpdateProjection();
	}

	glm::mat4 Camera::GetProjection() const
	{
		return this->projection;
	}

	void Camera::UpdateProjection()
	{
		this->projection = glm::perspective(fov, aspectRatio, near, far);
		this->projection[1][1] *= -1;
	}
}