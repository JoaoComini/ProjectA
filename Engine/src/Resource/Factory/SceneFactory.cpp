#include "SceneFactory.hpp"

namespace Engine
{
    void SceneFactory::Create(std::filesystem::path destination, Scene& scene)
    {

    }

    std::shared_ptr<Scene> SceneFactory::Load(std::filesystem::path source)
    {
        auto scene = std::make_shared<Scene>();


        return scene;
    }
};