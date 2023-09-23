#include "Scene.hpp"

#include "Components.hpp"

namespace Scene
{
    Entity Scene::CreateEntity()
    {
        return { registry.create(), &registry };
    }

    void Scene::Render(Rendering::Renderer& renderer)
    {

        auto view = registry.view<Component::Transform, Component::MeshRenderer>();

        view.each([&renderer](const auto& transform, const auto& mesh) {

            renderer.Draw(*(mesh.mesh), transform.Get());

        });
    }

}