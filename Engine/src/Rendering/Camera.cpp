#include "Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Engine
{
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: fov(fov), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip)
	{
		UpdateProjection();
	}

	void PerspectiveCamera::SetAspectRatio(float aspectRatio)
	{
		this->aspectRatio = aspectRatio;

		UpdateProjection();
	}

	glm::mat4 PerspectiveCamera::GetProjection() const
	{
		return this->projection;
	}

	float PerspectiveCamera::GetFov()
	{
		return fov;
	}

	void PerspectiveCamera::SetFov(float fov)
	{
		this->fov = fov;
		UpdateProjection();
	}

	float PerspectiveCamera::GetNear()
	{
		return nearClip;
	}

	void PerspectiveCamera::SetNear(float nearClip)
	{
		this->nearClip = nearClip;
		UpdateProjection();
	}

	float PerspectiveCamera::GetFar()
	{
		return farClip;
	}

	void PerspectiveCamera::SetFar(float farClip)
	{
		this->farClip = farClip;
		UpdateProjection();
	}

	void PerspectiveCamera::UpdateProjection()
	{
		this->projection = glm::perspective(fov, aspectRatio, nearClip, farClip);
		this->projection[1][1] *= -1;
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearClip, float farClip)
		: left(left), right(right), bottom(bottom), top(top), nearClip(nearClip), farClip(farClip)
	{
		projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
		projection[1][1] *= -1;
	}

	glm::mat4 OrthographicCamera::GetProjection() const
	{
		return projection;
	}
}