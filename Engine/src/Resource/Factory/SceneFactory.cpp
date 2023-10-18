#include "SceneFactory.hpp"

#include "Scene/SceneSerializer.hpp"

namespace Engine
{
    void SceneFactory::Create(std::filesystem::path destination, Scene& scene)
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(destination);
    }

    std::shared_ptr<Scene> SceneFactory::Load(std::filesystem::path source)
    {
        auto scene = std::make_shared<Scene>();

        SceneSerializer serializer(*scene);
        serializer.Deserialize(source);

        return scene;
    }
};