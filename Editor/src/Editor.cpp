#include "Core/Application.hpp"
#include "Core/Main.hpp"

#include "System/CameraSystem.hpp"

namespace Engine
{
    class Editor : public Application
    {
    public:
        Editor(ApplicationSpec& spec) : Application(spec)
        {
            AddSystem<CameraSystem>();
        }
    };

    std::unique_ptr<Application> CreateApplication()
    {
        ApplicationSpec spec
        {
            .name = "ProjectA"
        };

        return std::make_unique<Editor>(spec);
    }

}

