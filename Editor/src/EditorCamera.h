#pragma once

#include <Rendering/RenderCamera.h>
#include <Core/Input.h>

class EditorCamera : public Engine::RenderCamera
{
public:
	EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

	void Update(float timestep);
	void HandleKeyboardInput(Engine::Input& input, float timestep);
	void HandleMouseInput(Engine::Input& input, float timestep);

	void OnInputEvent(const Engine::InputEvent& event);

private:
	float yaw = 0.f, pitch = 0.f;
	
	glm::vec2 currentMousePosition{ 0.0f, 0.0f };

	glm::vec3 RightVector() const;
	glm::vec3 ForwardVector() const;
	glm::vec3 UpVector() const;
};