#include "InputSystem.hpp"

#include "Core/KeyCode.hpp"

#include <iostream>

InputSystem::InputSystem(Engine::Scene& scene): System(scene)
{
    input = Engine::Input::GetInstance();

    entity = scene.CreateEntity();

    auto& transform = entity.AddComponent<Engine::Component::Transform>();
    transform.position = glm::vec3(0, -10, 50);

    auto camera = Engine::Camera(glm::radians(45.f), (float)16 / 9, 0.1f, 2000.0f);
    entity.AddComponent<Engine::Component::Camera>(camera);
}

void InputSystem::Update(float timestep)
{
    if (input == nullptr)
    {
        return;
    }

    auto& transform = entity.GetComponent<Engine::Component::Transform>();

    if (input->IsKeyDown(Engine::KeyCode::W))
    {
        transform.position -= glm::vec3{ 0, 0, speed } * timestep;
    }

    if (input->IsKeyDown(Engine::KeyCode::S))
    {
        transform.position += glm::vec3{ 0, 0, speed } * timestep;
    }

    if (input->IsKeyDown(Engine::KeyCode::A))
    {
        transform.position -= glm::vec3{ speed, 0, 0 } * timestep;
    }

    if (input->IsKeyDown(Engine::KeyCode::D))
    {
        transform.position += glm::vec3{ speed, 0, 0 } * timestep;
    }

    if (input->IsKeyDown(Engine::KeyCode::LeftShift))
    {
        transform.position += glm::vec3{ 0, speed, 0 } *timestep;
    }

    if (input->IsKeyDown(Engine::KeyCode::LeftControl))
    {
        transform.position -= glm::vec3{ 0, speed, 0 } *timestep;
    }
}
