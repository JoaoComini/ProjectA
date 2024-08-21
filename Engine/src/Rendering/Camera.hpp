#pragma once

namespace Engine
{

	class Camera
	{
	public:
		enum Type
		{
			Pespective,
			Orthographic
		};

		Camera() = default;
		
		void SetPerspective(float fov, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetAspectRatio(float aspectRatio);

		void SetType(Type type);
		Type GetType() const;

		glm::mat4 GetProjection();

		float GetFov() const;
		void SetFov(float fov);

		float GetSize() const;
		void SetSize(float size);

		float GetNear() const;
		void SetNear(float nearClip);

		float GetFar() const;
		void SetFar(float farClip);

		template <class Archive>
		void Serialize(Archive& ar)
		{
			ar(type, fov, aspectRatio, size, nearClip, farClip);
		}

	private:
		void UpdateProjection();

		Type type{ Type::Pespective };

		// Perspective
		float fov{ glm::radians(45.f) };
		float aspectRatio{ 16.f / 9.f };

		// Orthographic
		float size{};
		
		float nearClip{ 1.f };
		float farClip{ 3000.f };

		bool dirty{ true };

		glm::mat4 projection{};
	};
}