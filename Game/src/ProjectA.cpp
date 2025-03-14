#include "Core/Application.h"
#include "Core/Main.h"


namespace Engine
{
    class ProjectA : public Application
    {
    public:
        ProjectA(ApplicationSpec& spec) : Application(spec)
        {
        }
    };

    std::unique_ptr<Application> CreateApplication(const ApplicationArgs &args)
    {
        ApplicationSpec spec
        {
            .name = "ProjectA",
            .args = args,
        };

        return std::make_unique<ProjectA>(spec);
    }

}

