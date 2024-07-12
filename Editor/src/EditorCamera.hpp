#pragma once

#include <Rendering/Camera.hpp>
#include <Core/Input.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class EditorCamera: public Engine::Camera
{
public:
	EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

	void Update(float timestep);
	void HandleKeyboardInput(Engine::Input& input, float timestep);
	void HandleMouseInput(Engine::Input& input, float timestep);

	glm::mat4 GetTransform();

private:
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };

	float yaw = 0.f, pitch = 0.f;
	
	glm::vec2 currentMousePosition{ 0.0f, 0.0f };

	glm::vec3 RightVector() const;
	glm::vec3 ForwardVector() const;
	glm::vec3 UpVector() const;

	glm::quat GetRotation() const;
};