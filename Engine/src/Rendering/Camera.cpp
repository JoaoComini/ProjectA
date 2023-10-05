#include "Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Engine
{
	Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip) : fov(fov), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip)
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

	float Camera::GetFov()
	{
		return fov;
	}

	void Camera::SetFov(float fov)
	{
		this->fov = fov;
		UpdateProjection();
	}

	float Camera::GetNear()
	{
		return nearClip;
	}

	void Camera::SetNear(float nearClip)
	{
		this->nearClip = nearClip;
		UpdateProjection();
	}

	float Camera::GetFar()
	{
		return farClip;
	}

	void Camera::SetFar(float farClip)
	{
		this->farClip = farClip;
		UpdateProjection();
	}

	void Camera::UpdateProjection()
	{
		this->projection = glm::perspective(fov, aspectRatio, nearClip, farClip);
		this->projection[1][1] *= -1;
	}
}