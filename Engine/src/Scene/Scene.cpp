#include "Scene.hpp"

namespace Engine
{
    Scene::Scene()
    {
        root = new Node();
        root->SetName("root");
        root->SetScene(this);
    }

    void Scene::OnUpdate(float timestep)
    {

    }

    Node* Scene::GetRoot() const
    {
        return root;
    }
}
