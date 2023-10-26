#pragma once

#include <glm/glm.hpp>

namespace Engine
{

	class Camera
	{
	public:
		virtual ~Camera() = default;
		virtual glm::mat4 GetProjection() const = 0;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void SetAspectRatio(float aspectRatio);

		glm::mat4 GetProjection() const override;

		float GetFov();
		void SetFov(float fov);

		float GetNear();
		void SetNear(float nearClip);

		float GetFar();
		void SetFar(float farClip);

	private:
		void UpdateProjection();

		float fov{};
		float aspectRatio{};
		float nearClip{};
		float farClip{};

		glm::mat4 projection{};
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float nearClip, float farClip);

		glm::mat4 GetProjection() const override;

		float left{};
		float right{};
		float bottom{};
		float top{};
		float nearClip{};
		float farClip{};

		glm::mat4 projection{};
	};
}