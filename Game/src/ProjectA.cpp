#include "Core/Application.hpp"
#include "Core/Main.hpp"

#include "System/InputSystem.hpp"

namespace Engine
{
    class ProjectA : public Application
    {
    public:
        ProjectA(ApplicationSpec& spec) : Application(spec)
        {
            AddSystem<InputSystem>();
        }
    };

    std::unique_ptr<Application> CreateApplication()
    {
        ApplicationSpec spec
        {
            .name = "ProjectA"
        };

        return std::make_unique<ProjectA>(spec);
    }

}

