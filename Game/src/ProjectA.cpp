#include "Core/Application.hpp"
#include "Core/Main.hpp"


namespace Engine
{
    class ProjectA : public Application
    {
    public:
        ProjectA(ApplicationSpec& spec) : Application(spec)
        {
        }
    };

    std::unique_ptr<Application> CreateApplication(ApplicationArgs args)
    {
        ApplicationSpec spec
        {
            .name = "ProjectA",
            .args = args,
        };

        return std::make_unique<ProjectA>(spec);
    }

}

