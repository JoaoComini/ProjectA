#include "SceneInstance.h"

#include "Scene/SceneResource.h"

namespace Engine::Component
{
    template<class Archive>
    void Save(Archive &ar, const SceneInstance& instance)
    {
        ar(instance.scene->GetId());
        ar(instance.local);
    }

    template
    void Save<cereal::PortableBinaryOutputArchive>(cereal::PortableBinaryOutputArchive&, const SceneInstance&);
}