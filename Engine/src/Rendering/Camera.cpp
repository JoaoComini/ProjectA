#include "Camera.hpp"

namespace Engine
{
	void Camera::SetPerspective(float fov, float nearClip, float farClip)
	{
		type = Type::Pespective;

		this->fov = fov;

		this->nearClip = nearClip;
		this->farClip = farClip;

		dirty = true;
	}

	void Camera::SetOrthographic(float size, float nearClip, float farClip)
	{
		type = Type::Orthographic;

		this->size = size;

		this->nearClip = nearClip;
		this->farClip = farClip;

		dirty = true;
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		this->aspectRatio = aspectRatio;

		dirty = true;
	}

	Camera::Type Camera::GetType() const
	{
		return type;
	}

	void Camera::SetType(Camera::Type type)
	{
		this->type = type;

		dirty = true;
	}

	float Camera::GetFov() const
	{
		return fov;
	}

	void Camera::SetFov(float fov)
	{
		this->fov = fov;

		dirty = true;
	}

	float Camera::GetNear() const
	{
		return nearClip;
	}

	void Camera::SetNear(float nearClip)
	{
		this->nearClip = nearClip;

		dirty = true;
	}

	float Camera::GetFar() const
	{
		return farClip;
	}

	void Camera::SetFar(float farClip)
	{
		this->farClip = farClip;

		dirty = true;
	}

	void Camera::UpdateProjection()
	{
		if (! dirty)
		{
			return;
		}

		switch (type)
		{
		case Engine::Camera::Pespective:
			projection = glm::perspective(fov, aspectRatio, farClip, nearClip);
			break;
		case Engine::Camera::Orthographic:
			float left = -size * aspectRatio * 0.5f;
			float right = size * aspectRatio * 0.5f;
			float bottom = -size * 0.5f;
			float top = size * 0.5f;

			projection = glm::ortho(left, right, bottom, top, farClip, nearClip);
			break;
		}

		projection[1][1] *= -1;

		dirty = false;
	}

	glm::mat4 Camera::GetProjection()
	{
		UpdateProjection();

		return projection;
	}
}