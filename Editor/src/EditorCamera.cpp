#include "EditorCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : Engine::PerspectiveCamera(fov, aspectRatio, nearClip, farClip)
{
}

void EditorCamera::Update(float timestep)
{
    Engine::Input& input = Engine::Input::Get();

    HandleMouseInput(input, timestep);
    HandleKeyboardInput(input, timestep);
}

void EditorCamera::HandleMouseInput(Engine::Input& input, float timestep)
{
    const auto& mousePosition = input.GetMousePosition();
    glm::vec2 mouseDelta = (mousePosition - currentMousePosition) * timestep;
    currentMousePosition = mousePosition;

    if (input.IsMouseButtonDown(Engine::MouseCode::ButtonRight))
    {
        float sign = UpVector().y < 0 ? -1.0f : 1.0f;

        yaw += mouseDelta.x;
        pitch += mouseDelta.y;
    }
}

void EditorCamera::HandleKeyboardInput(Engine::Input& input, float timestep)
{
    float speed = 25 * timestep;

    if (input.IsKeyDown(Engine::KeyCode::W))
    {
        position += ForwardVector() * speed;
    }

    if (input.IsKeyDown(Engine::KeyCode::S))
    {
        position -= ForwardVector() * speed;
    }

    if (input.IsKeyDown(Engine::KeyCode::A))
    {
        position -= RightVector() * speed;
    }

    if (input.IsKeyDown(Engine::KeyCode::D))
    {
        position += RightVector() * speed;
    }

    if (input.IsKeyDown(Engine::KeyCode::LeftShift))
    {
        position += UpVector() * speed;
    }

    if (input.IsKeyDown(Engine::KeyCode::LeftControl))
    {
        position -= UpVector() * speed;
    }
}

glm::mat4 EditorCamera::GetTransform()
{
    return glm::translate(glm::mat4(1.0f), position)
        * glm::mat4_cast(GetRotation());
}

glm::vec3 EditorCamera::RightVector() const
{
    return glm::rotate(GetRotation(), glm::vec3{1.f, 0.f, 0.f});
}

glm::vec3 EditorCamera::ForwardVector() const
{
    return glm::rotate(GetRotation(), glm::vec3{ 0.0f, 0.f, -1.f });
}

glm::vec3 EditorCamera::UpVector() const
{
    return glm::rotate(GetRotation(), glm::vec3{0.f, 1.f, 0.f});
}

glm::quat EditorCamera::GetRotation() const
{
    return glm::quat(glm::vec3{ -pitch, -yaw, 0.0f });
}
