#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);
		~Camera() = default;

		void SetAspectRatio(float aspectRatio);

		glm::mat4 GetProjection() const;

		float GetFov();
		void SetFov(float fov);

		float GetNear();
		void SetNear(float nearClip);

		float GetFar();
		void SetFar(float farClip);

	private:
		void UpdateProjection();

		float fov;
		float aspectRatio;
		float nearClip;
		float farClip;

		glm::mat4 projection;
	};
}