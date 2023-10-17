#include "CameraSystem.hpp"

#include "Core/KeyCode.hpp"

#include <iostream>

CameraSystem::CameraSystem(Engine::Scene& scene): System(scene)
{
    input = &Engine::Input::Get();
}

void CameraSystem::Update(float timestep)
{
    if (input == nullptr)
    {
        return;
    }

    auto it = scene.FindFirstEntity<Engine::Component::Camera, Engine::Component::Transform>();
    
    Engine::Entity entity = it.first;

    if (!it.second)
    {
        entity = scene.CreateEntity();

        auto& transform = entity.AddComponent<Engine::Component::Transform>();
        transform.position = glm::vec3(0, -10, 50);

        auto camera = Engine::Camera(glm::radians(60.f), (float)16 / 9, 0.1f, 2000.0f);
        entity.AddComponent<Engine::Component::Camera>(camera);

        entity.GetComponent<Engine::Component::Name>().name = "Main Camera";
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
